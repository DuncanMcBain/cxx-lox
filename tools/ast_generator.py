#! /usr/bin/env python

from pathlib import Path

import sys


def defineConstructor(classname, fields):
    ret = []
    args = list(map(lambda x: x[1].rstrip('_'), fields))
    ret.append(classname + '(' +
               ', '.join(map(lambda x, y: x[0] + " " + y, fields, args)) + ')\n')
    sep = ':'  # first iteration only
    for (field, arg) in zip(fields, args):
        ret.append('{0}{1}({2})\n'.format(sep, field[1], arg.rstrip('_')))
        sep = ','  # every other ctor var preceded by a comma
    ret.append('{}\n')
    return ret


def declareType(basename, classname):
    ret = []
    ret.append('struct {0};\n'.format(classname))
    return ret


def defineType(basename, classname, fields):
    ret = []
    ret.append('struct {0} : {1} {{\n'.format(classname, basename))
    for field in fields:
        ret.append('{} {};\n'.format(*field))
    ret.extend(defineConstructor(classname, fields))
    ret.append('void accept(Visitor& v) override')
    ret.append('{{ v.visit{}{}(*this); }}'.format(classname, basename))
    ret.append('};\n\n')
    return ret


def defineVisitor(basename, types):
    ret = []
    ret.append('struct Visitor {\n')
    for typ in types:
        ret.append('void visit{0}{1}({0}&);\n'.format(typ, basename))
    ret.append('};\n\n')
    return ret


def defineAST(dir, basename, types):
    lines = []
    lines.append('#ifndef LOX_{0}_H_\n#define LOX_{0}_H_\n\n'.format(
        basename.upper()))
    lines.append('#include "Token.h"\n\n')
    lines.append('#include <string>\n\n')
    lines.append('namespace lox {\n\n')
    lines.append('struct Visitor;')
    lines.append('struct {} {{ virtual void accept(Visitor&); }};'.format(
        basename))
    for typ in iter(types):
        lines.extend(declareType(basename, typ))
    lines.extend(defineVisitor(basename, iter(types)))
    for typ in iter(types):
        lines.extend(defineType(basename, typ, types[typ]))
    lines.append('}  // namespace lox\n#endif\n')
    path = Path(dir) / (basename + ".h")
    with open(path, 'w') as f:
        for line in lines:
            f.write(line)
    return


def main():
    if len(sys.argv) != 2:
        print("Usage: {0} <directory>\n".format(sys.argv[0]))
    out_dir = sys.argv[1]
    classes = {
        "Binary": [("Expr&", "left_"), ("Expr&", "right_"), ("Token", "op_")],
        "Grouping"  : [("Expr&", "expr_")],
        "StrLiteral": [("std::string", "value_")],
        "NumLiteral": [("double", "literal_")],
        "Unary"     : [("Expr&", "right_"), ("Token", "op_")]
    }
    defineAST(out_dir, "Expr", classes)


if __name__ == "__main__":
    main()
