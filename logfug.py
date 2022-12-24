import re
import operator
import sys
import argparse

precedence = {
    '~' : 2,
    '*' : 1,
    '+' : 0,
}

def next_token(expr, idx):
    idx+=1

    if (idx == len(expr)):
        return None

    while (idx < len(expr) and expr[idx] == ' '):
        idx+=1

    return (expr[idx], idx)


def tokenize(expr):
    i = -1
    while True:

        val = next_token(expr, i)
        if not val:
            break
        token,i = val[0], val[1]
        yield(token)

        val = next_token(expr, i)
        if not val:
            continue
        ntoken = val[0]
        if (re.match("\)[~\[a-d\]\(]|[a-d][\[a-d\]~]", token+ntoken)):
            yield('*')

def parse_RPN(expr):

    tokens = tokenize(expr)

    operators = []
    RPN = []

    for i, token in enumerate(tokens):
        if token in "abcd":
            RPN.append(token)

        elif (token == "("):
            operators.append(token)

        elif (token == ")"):
            top = operators[-1] if operators else None
            while top and top != "(":
                RPN.append(operators.pop())
                top = operators[-1] if operators else None
            if top!='(':
                print("\nERROR: Mismatched parantheses!\n")
                exit(1)
            operators.pop()

        else:
            top = operators[-1] if operators else None
            while top and top != "(" and precedence[top] > precedence[token]:
                RPN.append(operators.pop())
                top = operators[-1] if operators else None
            operators.append(token)


    while operators:
        op = operators.pop()
        if (op == '('):
            print("\nERROR: Mismatched parantheses!\n")
            exit(1)
        RPN.append(op)

    return RPN
            
def solve_RPN(rpn, subst):
    stack = []
    for e in rpn:
        if e not in "*+~":
            stack.append(bool(subst[e]))
        elif e == '~':
            stack.append(not stack.pop())
        else:
            a = stack.pop()
            b = stack.pop()
            if e == '+':
                stack.append(a or b)
            elif e == '*':
                stack.append(a and b)
    return stack[0]

def truthtable(rpn, reverse):
    subst = {
        'a':0,
        'b':0,
        'c':0,
        'd':0
    }
    print(f"\n-- {'DCBA' if reverse else 'ABCD'} -")

    for i in range(16):
        print("{:02d}".format(i), end=' ')
        n = bin(i)[2:].zfill(4)
        print(n, end=' ')
        if (reverse):
            subst['a'] = int(n[3])
            subst['b'] = int(n[2])
            subst['c'] = int(n[1])
            subst['d'] = int(n[0])
        else:
            subst['a'] = int(n[0])
            subst['b'] = int(n[1])
            subst['c'] = int(n[2])
            subst['d'] = int(n[3])
        print(int(solve_RPN(rpn, subst)))
    print("-"*9,end="\n\n")


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        prog = "logfug",
        description = "Display the solution to a Boolean-algebra expression as a truthtable.",
        epilog = "shoutout to reverse polish notation"
    )

    parser.add_argument("expression")
    parser.add_argument("-r", "--reverse", action='store_true', help="use DCBA order intstead of ABCD")

    args = parser.parse_args()

    rpn = parse_RPN(args.expression)
    truthtable(rpn, args.reverse)
