from setuptools import setup

setup(
    name='MDSplus',
    version='7.139.40',
    description='MDSplus Python Objects',
    url='https://www.mdsplus.org/',
    author='MDSplus Developers',
    license='MIT',
    classifiers=['License :: OSI Approved :: MIT License', 'Programming Language :: Python :: 3'],
    keywords=['physics', 'mdsplus', ],
    long_description=(
            "This module provides all of the functionality of MDSplus TDI natively in python.\n"
            "All of the MDSplus data types such as signal are represented as python classes.\n"
        ),
    packages=['MDSplus'],
    package_data={'MDSplus':['*.py']},
)
