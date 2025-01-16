
import os
import glob
import shutil

import subprocess

# For compatability with python < 3.3
if not hasattr(shutil, 'which'):
    def which(cmd):
        try:
            which_result = subprocess.check_output(['which', cmd])
        except subprocess.CalledProcessError:
            return None
        return which_result.strip()

    shutil.which = which

# Move to the root of the repository
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

have_tools = True

# Yacc/Bison Parser Generator

if 'YACC' in os.environ:
    yacc = os.environ['YACC']
else:
    yacc = shutil.which('bison')

if yacc is None:
    yacc = shutil.which('yacc')

if yacc is None:
    print('Please install bison or yacc, or set the YACC environment variable')
    have_tools = False

# Flex/Lex Lexical Analyser Generator

if 'LEX' in os.environ:
    lex = os.environ['LEX']
else:
    lex = shutil.which('flex')

if lex is None:
    lex = shutil.which('lex')

if lex is None:
    print('Please install flex or lex, or set the LEX environment variable')
    have_tools = False

if not have_tools:
    exit(1)

yacc_filename_list = glob.glob('**/*.y')
lex_filename_list = glob.glob('**/*.l')

for yacc_filename in yacc_filename_list:
    print("Running '{} --verbose {}'".format(yacc, yacc_filename))
    status = subprocess.Popen([yacc, '--verbose', yacc_filename]).wait()
    if status != 0:
        print("Failed to process '{}'".format(yacc_filename))
        exit(1)

for lex_filename in lex_filename_list:
    print("Running '{} -p {}'".format(lex, lex_filename))
    status = subprocess.Popen([lex, '-p', lex_filename]).wait()
    if status != 0:
        print("Failed to process '{}'".format(lex_filename))
        exit(1)
