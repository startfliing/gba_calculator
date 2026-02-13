#include "calculator.hpp"

Calculator::Calculator() : token_count(0) {}

bool Calculator::isOperator(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool Calculator::isNumber(char c) const {
    return (c >= '0' && c <= '9') || c == '.';
}

double Calculator::parseNumber(const char* expr, int& i) {
    double num = 0;
    double decimal_place = 1;
    bool is_decimal = false;

    while (expr[i] != '\0' && (isNumber(expr[i]) || expr[i] == '.')) {
        if (expr[i] == '.') {
            if (is_decimal) break;  // Second decimal point, stop parsing
            is_decimal = true;
            i++;
            continue;
        }

        if (is_decimal) {
            decimal_place *= 0.1;
            num += (expr[i] - '0') * decimal_place;
        } else {
            num = num * 10 + (expr[i] - '0');
        }
        i++;
    }
    i--;  // Back up one position since the loop will increment i
    return num;
}

int Calculator::getOperatorPrecedence(char op) const {
    switch(op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        case '^':
            return 3;
        default:
            return -1;
    }
}

void Calculator::infixToRPN(const char* infix_expression) {
    token_count = 0;
    Token output_queue[MAX_TOKENS];
    int output_count = 0;
    Token operator_stack[MAX_TOKENS];
    int operator_stack_count = 0;

    int i = 0;
    while (infix_expression[i] != '\0') {
        // Skip whitespace
        if (infix_expression[i] == ' ') {
            i++;
            continue;
        }

        // Handle numbers
        if (isNumber(infix_expression[i])) {
            double num = parseNumber(infix_expression, i);
            output_queue[output_count++] = Token(num);
        }
        // Handle operators
        else if (isOperator(infix_expression[i])) {
            Token current_op(infix_expression[i]);
            
            // Pop operators from stack to output based on precedence and associativity
            while (operator_stack_count > 0) {
                Token top_op = operator_stack[operator_stack_count - 1];
                
                // Check if top is an operator (not a parenthesis)
                if (top_op.type == Token::OPERATOR) {
                    bool should_pop = false;
                    
                    if (current_op.isLeftAssociative()) {
                        should_pop = top_op.precedence >= current_op.precedence;
                    } else {
                        should_pop = top_op.precedence > current_op.precedence;
                    }
                    
                    if (should_pop) {
                        output_queue[output_count++] = operator_stack[--operator_stack_count];
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
            
            operator_stack[operator_stack_count++] = current_op;
        }
        // Handle left parenthesis
        else if (infix_expression[i] == '(') {
            operator_stack[operator_stack_count++] = Token('(', true);
        }
        // Handle right parenthesis
        else if (infix_expression[i] == ')') {
            // Pop everything until matching left parenthesis
            while (operator_stack_count > 0) {
                Token top = operator_stack[--operator_stack_count];
                if (top.type == Token::LPAREN) {
                    break;
                }
                output_queue[output_count++] = top;
            }
        }

        i++;
    }

    // Pop remaining operators
    while (operator_stack_count > 0) {
        output_queue[output_count++] = operator_stack[--operator_stack_count];
    }

    // Copy output to tokens array
    for (int j = 0; j < output_count && j < MAX_TOKENS; j++) {
        tokens[j] = output_queue[j];
        token_count++;
    }
}

double Calculator::evaluateRPN() {
    double eval_stack[MAX_TOKENS];
    int stack_count = 0;

    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == Token::NUMBER) {
            eval_stack[stack_count++] = tokens[i].value;
        }
        else if (tokens[i].type == Token::OPERATOR) {
            if (stack_count < 2) return 0;  // Error: not enough operands
            
            double b = eval_stack[--stack_count];
            double a = eval_stack[--stack_count];
            double result = 0;

            switch(tokens[i].op) {
                case '+':
                    result = a + b;
                    break;
                case '-':
                    result = a - b;
                    break;
                case '*':
                    result = a * b;
                    break;
                case '/':
                    result = (b != 0) ? a / b : 0;  // Avoid division by zero
                    break;
                case '^': {
                    // Simple power calculation
                    result = 1;
                    int exp = (int)b;
                    bool negative_exp = exp < 0;
                    exp = negative_exp ? -exp : exp;
                    
                    for (int j = 0; j < exp; j++) {
                        result *= a;
                    }
                    if (negative_exp) result = 1.0 / result;
                    break;
                }
                default:
                    result = 0;
            }

            eval_stack[stack_count++] = result;
        }
    }

    return (stack_count > 0) ? eval_stack[0] : 0;
}

double Calculator::calculate(const char* infix_expression) {
    infixToRPN(infix_expression);
    return evaluateRPN();
}
