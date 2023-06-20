
import glob

yacc_file_list = glob.glob('**/*.y')
lex_file_list = glob.glob('**/*.l')

print(yacc_file_list)
print(lex_file_list)