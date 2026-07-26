#!/usr/bin/env python
"""Build the MDSplus "local" wheel using only the standard library.

A wheel is just a zip with a <name>-<version>.dist-info/ directory, so there is
no need for a build backend (hatchling/setuptools), pip build isolation, or any
network access. The wheel ships two files, both at the archive root so they land
in site-packages:

    MDSplus.pth           -> "import _mdsplus_bootstrap"
    _mdsplus_bootstrap.py -> resolves this install at interpreter startup

Installing the wheel makes `import MDSplus` resolve to the system-installed
python/MDSplus package. This is built at install time so the real install prefix
can be baked into the bootstrap (see the CMake install script).

Runtime dependencies are copied from python/MDSplus/pyproject.toml so they never
drift from the real package.

Usage:
    make_wheel.py SOURCE_TOML PTH_FILE BOOTSTRAP_TEMPLATE OUTDIR VERSION PREFIX
"""
import base64
import glob
import hashlib
import os
import sys
import zipfile

# Universal pure-Python wheel: filename carries the compressed "py2.py3-none-any"
# tag, which the WHEEL metadata expands to one Tag line each.
NAME = "MDSplus"
DISTNAME = "mdsplus"  # normalized name used in the filename and dist-info dir
TAG = "py2.py3-none-any"
WHEEL_TAGS = ("py2-none-any", "py3-none-any")
ZIP_DATE = (2020, 1, 1, 0, 0, 0)  # fixed, for reproducible archives


def load_project(path):
    try:
        import tomllib as toml_mod
    except ImportError:
        try:
            import tomli as toml_mod
        except ImportError:
            sys.exit(
                "make_wheel.py: reading %s needs Python 3.11+ (tomllib) or the "
                "'tomli' package installed for %s" % (path, sys.executable)
            )
    with open(path, "rb") as fp:
        return toml_mod.load(fp).get("project", {})


def record_hash(data):
    digest = hashlib.sha256(data).digest()
    return "sha256=" + base64.urlsafe_b64encode(digest).decode("ascii").rstrip("=")


def build_metadata(project, version):
    """Mirror the real package's [project] metadata so that `pip show`,
    importlib.metadata, etc. report the genuine MDSplus info -- not a stub.
    Only the version is overridden (with the CMake build version)."""
    lines = [
        "Metadata-Version: 2.1",
        "Name: %s" % NAME,
        "Version: %s" % version,
    ]

    if project.get("description"):
        lines.append("Summary: %s" % project["description"])
    if project.get("requires-python"):
        lines.append("Requires-Python: %s" % project["requires-python"])

    license = project.get("license")
    if isinstance(license, dict):
        license = license.get("text")
    if license:
        lines.append("License: %s" % license)

    # authors -> Author-email ("Name <email>") and/or Author.
    author_emails, author_names = [], []
    for author in project.get("authors", []):
        name, email = author.get("name"), author.get("email")
        if email and name:
            author_emails.append("%s <%s>" % (name, email))
        elif email:
            author_emails.append(email)
        elif name:
            author_names.append(name)
    if author_emails:
        lines.append("Author-email: %s" % ", ".join(author_emails))
    if author_names:
        lines.append("Author: %s" % ", ".join(author_names))

    if project.get("keywords"):
        lines.append("Keywords: %s" % ",".join(project["keywords"]))
    for classifier in project.get("classifiers", []):
        lines.append("Classifier: %s" % classifier)
    for label, url in project.get("urls", {}).items():
        lines.append("Project-URL: %s, %s" % (label, url))

    for dep in project.get("dependencies", []):
        lines.append("Requires-Dist: %s" % dep)
    for extra, extra_deps in project.get("optional-dependencies", {}).items():
        lines.append("Provides-Extra: %s" % extra)
        for dep in extra_deps:
            lines.append("Requires-Dist: %s; extra == '%s'" % (dep, extra))
    return ("\n".join(lines) + "\n").encode("utf-8")


def build_wheel_metadata():
    lines = [
        "Wheel-Version: 1.0",
        "Generator: mdsplus-cmake make_wheel.py",
        "Root-Is-Purelib: true",
    ]
    lines += ["Tag: %s" % tag for tag in WHEEL_TAGS]
    return ("\n".join(lines) + "\n").encode("utf-8")


def main():
    (
        source_toml,
        pth_file,
        bootstrap_template,
        outdir,
        version,
        prefix,
    ) = sys.argv[1:7]

    project = load_project(source_toml)
    distinfo = "%s-%s.dist-info" % (DISTNAME, version)

    with open(pth_file, "rb") as fp:
        pth_data = fp.read()

    # Bake this install's prefix into the bootstrap as a valid Python literal.
    with open(bootstrap_template, "r") as fp:
        bootstrap = fp.read().replace("@MDSPLUS_PREFIX@", repr(prefix))

    # (arcname, bytes) for every member except RECORD itself.
    members = [
        ("%s.pth" % NAME, pth_data),
        ("_mdsplus_bootstrap.py", bootstrap.encode("utf-8")),
        ("%s/METADATA" % distinfo, build_metadata(project, version)),
        ("%s/WHEEL" % distinfo, build_wheel_metadata()),
    ]

    record_lines = [
        "%s,%s,%d" % (arc, record_hash(data), len(data)) for arc, data in members
    ]
    record_lines.append("%s/RECORD,," % distinfo)  # RECORD lists itself, unhashed
    members.append(
        ("%s/RECORD" % distinfo, ("\n".join(record_lines) + "\n").encode("utf-8"))
    )

    if not os.path.isdir(outdir):
        os.makedirs(outdir)

    # Remove any stale MDSplus wheels so only the current one remains.
    for pattern in ("mdsplus-*.whl", "MDSplus-*.whl"):
        for old in glob.glob(os.path.join(outdir, pattern)):
            os.remove(old)

    wheel_path = os.path.join(outdir, "%s-%s-%s.whl" % (DISTNAME, version, TAG))
    with zipfile.ZipFile(wheel_path, "w", zipfile.ZIP_DEFLATED) as whl:
        for arc, data in members:
            info = zipfile.ZipInfo(arc, date_time=ZIP_DATE)
            info.external_attr = 0o644 << 16
            whl.writestr(info, data)

    sys.stdout.write(wheel_path + "\n")


if __name__ == "__main__":
    main()
