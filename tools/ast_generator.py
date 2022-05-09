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
    ret.append('ExprResult accept(Visitor& v) override')
    ret.append('{{ return v.visit{}{}(*this); }}'.format(classname, basename))
    ret.append('};\n\n')
    return ret


def defineVisitor(basename, types):
    ret = []
    ret.append('struct Visitor {\n')
    for typ in types:
        ret.append('virtual ExprResult visit{0}{1}({0}&);\n'.format(typ, basename))
    ret.append('};\n\n')
    return ret


def defineAST(dir, basename, types):
    lines = []
    lines.append('#ifndef LOX_{0}_H_\n#define LOX_{0}_H_\n\n'.format(
        basename.upper()))
    lines.append('#include "Token.h"\n\n')
    lines.append('#include <cstddef>\n')
    lines.append('#include <memory>\n')
    lines.append('#include <string>\n')
    lines.append('#include <variant>\n\n')
    lines.append('using ExprResult = std::variant<bool, double, std::string, std::nullptr_t>;\n\n')
    lines.append('namespace lox {\n\n')
    lines.append('struct Visitor;\n\n')
    lines.append('struct {} {{\nvirtual ExprResult accept(Visitor&) = 0;\n'.format(
        basename))
    lines.append('virtual ~{}() = default;\n}};\n\n'.format(basename))
    for typ in iter(types):
        lines.extend(declareType(basename, typ))
    lines.append('\n')
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
        return 1
    out_dir = sys.argv[1]
    classes = {
        "Binary"     : [("std::shared_ptr<Expr>", "left_"), ("std::shared_ptr<Expr>", "right_"), ("Token", "op_")],
        "Grouping"   : [("std::shared_ptr<Expr>", "expr_")],
        "BoolLiteral": [("bool", "value_")],
        "StrLiteral" : [("std::string", "value_")],
        "NullLiteral": [],
        "NumLiteral" : [("double", "value_")],
        "Unary"      : [("std::shared_ptr<Expr>", "right_"), ("Token", "op_")]
    }
    defineAST(out_dir, "Expr", classes)


if __name__ == "__main__":
    main()
