#! /usr/bin/env python

from pathlib import Path

import sys


# TODO: Improve this. It is so far the only class to be constructed differently
# but I should still fix that in a more satisfying manner than special-casing
def defineBlock(classname, fields):
    ret = []
    args = list(map(lambda x: x[1].rstrip('_'), fields))
    ret.append(classname + '(' +
               ', '.join(map(lambda x, y: x[0] + " &&" + y, fields, args)) + ')\n')
    sep = ':'
    for (field, arg) in zip(fields, args):
        ret.append('{0}{1}(std::move({2}))\n'.format(sep, field[1], arg.rstrip('_')))
        sep = ','
    ret.append('{}\n')
    return ret


def defineConstructor(classname, fields):
    if classname == "Block":
        return defineBlock(classname, fields)
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
    return_type = 'void' if basename == 'Stmt' else 'ExprResult'
    ret.append('{0} accept({1}::Visitor<{0}>& v) override'.format(return_type, basename.lower()))
    ret.append('{{ return v.visit{}{}(*this); }}'.format(classname, basename))
    ret.append('std::string accept({0}::Visitor<std::string>& v) override'.format(basename.lower()))
    ret.append('{{ return v.visit{}{}(*this); }}'.format(classname, basename))
    ret.append('};\n\n')
    return ret


def defineVisitor(basename, types):
    ret = []
    ret.append('namespace {}{{\n\n'.format(basename.lower()))
    ret.append('template <typename T> struct Visitor {\n')
    return_type = 'void' if basename == 'Stmt' else 'ExprResult'
    for typ in types:
        ret.append('virtual T visit{0}{1}({0}&) = 0;\n'.format(typ, basename))
    ret.append('virtual ~Visitor() = default;')
    ret.append('};\n\n')
    ret.append('}}  // namespace {}\n\n'.format(basename.lower()))
    return ret


def defineAST(dir, basename, types):
    lines = []
    return_type = 'void' if basename == 'Stmt' else 'ExprResult'
    lines.append('#ifndef LOX_{0}_H_\n#define LOX_{0}_H_\n\n'.format(
        basename.upper()))
    if not basename == "Expr":
        lines.append('#include "Expr.h"\n')
    lines.append('#include "Token.h"\n\n')
    lines.append('#include <absl/container/inlined_vector.h>\n\n')
    lines.append('#include <cstddef>\n')
    lines.append('#include <memory>\n')
    lines.append('#include <string>\n')
    lines.append('#include <variant>\n\n')
    lines.append('namespace lox {\n\n')
    if basename == "Expr":
        lines.append('class Callable;\n')
        lines.append('using CallablePtr = std::shared_ptr<Callable>;\n\n')
        lines.append('using ExprResult = std::variant<bool, double, std::string, std::nullptr_t, CallablePtr>;\n\n')
    lines.append('namespace {} {{\n\n'.format(basename.lower()))
    lines.append('template <typename T> struct Visitor;\n\n')
    lines.append('}}  // namespace {}\n\n'.format(basename.lower()))
    lines.append('struct {0} {{\nvirtual {1} accept({2}::Visitor<{1}>&) = 0;\n'.format(
        basename, return_type, basename.lower()))
    lines.append('virtual {0} accept({1}::Visitor<{0}>&) = 0;\n'.format(
        'std::string',  basename.lower()))
    lines.append('virtual ~{}() = default;\n}};\n\n'.format(basename))
    for typ in iter(types):
        lines.extend(declareType(basename, typ))
    lines.append('\n')
    lines.extend(defineVisitor(basename, iter(types)))
    lines.append('using {0}Ptr = std::shared_ptr<{0}>;'.format(basename))
    if basename == "Stmt":
        lines.append('using StatementsList = absl::InlinedVector<std::shared_ptr<lox::Stmt>, 8>;\n\n')
        lines.append('using TokensList = absl::InlinedVector<Token, 8>;\n\n')
    if basename == "Expr":
        lines.append('using ExpressionsList = absl::InlinedVector<std::shared_ptr<lox::Expr>, 8>;\n\n')
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
        "Assign"     : [("Token", "name_"), ("ExprPtr", "val_")],
        "Binary"     : [("ExprPtr", "left_"), ("ExprPtr", "right_"), ("Token", "op_")],
        "Ternary"    : [("ExprPtr", "cond_"), ("ExprPtr", "left_"), ("ExprPtr", "right_")],
        "Call"       : [("ExprPtr", "callee_"), ("Token", "paren_"), ("ExpressionsList", "args_")],
        "Group"      : [("ExprPtr", "expr_")],
        "BoolLiteral": [("bool", "value_")],
        "StrLiteral" : [("std::string", "value_")],
        "NullLiteral": [],
        "NumLiteral" : [("double", "value_")],
        "Logical"    : [("ExprPtr", "left_"), ("ExprPtr", "right_"), ("Token", "op_")],
        "Variable"   : [("Token", "name_")],
        "Unary"      : [("ExprPtr", "right_"), ("Token", "op_")]
    }
    stmt_classes = {
        "Block"     : [("StatementsList", "statements_")],
        "Expression": [("ExprPtr", "expression_")],
        "Fn"        : [("Token", "name_"), ("TokensList", "tokens_"), ("StatementsList", "statements_")],
        "If"        : [("ExprPtr", "condition_"), ("StmtPtr", "then_"), ("StmtPtr", "else_br_")],
        "While"     : [("ExprPtr", "condition_"), ("StmtPtr", "body_")],
        "Var"       : [("Token", "name_"), ("ExprPtr", "initialiser_")],
    }
    defineAST(out_dir, "Expr", classes)
    defineAST(out_dir, "Stmt", stmt_classes)


if __name__ == "__main__":
    main()
