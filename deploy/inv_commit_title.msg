You have specified an invalid title for the commit mentioned in the subject of this email.
This is only a warning for now but in the not too distant future we will be requiring
properly formatted commit messages. The commit message should have the following format:

<commit-type>: <commit-title>
<blank line>
<commit details>

Where:

<commit-type> is one of (note the type must be followed by a colon with no spaces):

  Feature  (commit is introducing a new feature to MDSplus. 
            Will eventually trigger "minor" version increment.)
  Fix      (commit is to fix an existing bug in MDSplus.
            Will eventually trigger "path" version increment.)
  Build    (commit affects build/deploy scripts and configurations.
            Does not trigger a new release version.)
  Docs     (commit affects MDSplus documentation.
            Does not trigger a new release version.)
  Tests    (changes to or additional regression tests.
            Does not trigger a new release version.)

The check on the <commit-type> is case insensitive.

<commit-title> is a short but meaningful title of the change being made.
<commit-details> is one or more lines describing the commit in detail. While
                 the purpose and details of the commit are fully understood
                 by the author of the commit at the time of the commit it
                 may not be that obvious to other developers and even less
                 so as time passes.

Eventually it is planned to automatically generate a formatted release notes
document from the commit messages. If you are adding a series of commits
to add a new feature or fix a bug etc. it would be best to either squash
the commits into one commit or use the same commit type and title in the
commits so the release notes generator can consolidate the details.
 
