
import os
import glob
import shutil

from subprocess import *

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
    print(f"Running '{yacc} --verbose {yacc_filename}'")
    status = Popen([yacc, '--verbose', yacc_filename]).wait()
    if status != 0:
        print(f"Failed to process '{yacc_filename}'")
        exit(1)

for lex_filename in lex_filename_list:
    print(f"Running '{lex} -p {lex_filename}'")
    status = Popen([lex, '-p', lex_filename]).wait()
    if status != 0:
        print(f"Failed to process '{lex_filename}'")
        exit(1)
