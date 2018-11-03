import os, argparse, re

pragma_once_files = set()

def line_equal(a : str, b : str):
    return str_equalize(a) == str_equalize(b)

def str_equalize(line : str):
    return re.sub(r'\s\s+', ' ', line.strip())

def read_include(line : str):
    return re.findall(r'#include\s?"([^>]+)"', line)

def get_included_headers(filename : str):
    f = open(filename, 'r', encoding='utf-8')
    included = []
    for line in f:
        if line_equal(line, "#pragma once"):
            pragma_once_files.add(filename)
        else:
            file_list = read_include(str_equalize(line))
            if len(file_list) > 0:
                included.append(file_list[0])
    
    return included

def get_header_combined(filename : str):
    f = open(filename, 'r', encoding='utf-8')
    content = f.read()
    included_headers = get_included_headers(filename)
    for header in included_headers:
        if header not in pragma_once_files:
            combined_content = get_header_combined(header)
            content = re.sub(r'#include\s?"(%s)"' % header, lambda _: combined_content, content)
        else:
            content = re.sub(r'#include\s?"(%s)"' % header, '', content)
    return content
            

parser = argparse.ArgumentParser()
parser.add_argument("--directory", default=".", type=str)
parser.add_argument("--target", default="target.h", type=str)
parser.add_argument("--output", default="target_combined.h", type=str)

args = parser.parse_args()

old_path = os.path.abspath(os.curdir)
os.chdir(args.directory)

content = get_header_combined(args.target)

os.chdir(old_path)
o = open(args.output, 'w', encoding='utf-8')

o.write(content)
o.close()

