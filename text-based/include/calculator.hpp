#ifndef __CALCULATOR__
#define __CALCULATOR__

// Token class represents operators and numeric values
class Token {
public:
    enum TokenType {
        NUMBER,
        OPERATOR,
        LPAREN,
        RPAREN
    };

    TokenType type;
    double value;          // Used for NUMBER tokens
    char op;               // Used for OPERATOR tokens (+, -, *, /, ^)
    int precedence;        // Used for OPERATOR tokens

    // Constructor for number token
    Token(double val) : type(NUMBER), value(val), op('\0'), precedence(-1) {}

    // Constructor for operator token
    Token(char operator_char) : type(OPERATOR), value(0), op(operator_char), precedence(0) {
        setPrecedence();
    }

    // Constructor for parenthesis token
    Token(char paren, bool is_paren) 
        : type(paren == '(' ? LPAREN : RPAREN), value(0), op(paren), precedence(-1) {}

    // Default constructor
    Token() : type(NUMBER), value(0), op('\0'), precedence(-1) {}

    // Set operator precedence
    void setPrecedence() {
        switch(op) {
            case '+':
            case '-':
                precedence = 1;
                break;
            case '*':
            case '/':
                precedence = 2;
                break;
            case '^':
                precedence = 3;
                break;
            default:
                precedence = -1;
        }
    }

    // Check if operator is left associative
    bool isLeftAssociative() const {
        return op != '^';  // ^ (power) is right associative
    }
};

// Calculator class that processes infix notation and uses shunting yard algorithm
class Calculator {
private:
    static const int MAX_TOKENS = 100;
    Token tokens[MAX_TOKENS];
    int token_count;

    // Helper functions
    bool isOperator(char c) const;
    bool isNumber(char c) const;
    double parseNumber(const char* expr, int& i);
    int getOperatorPrecedence(char op) const;

public:
    Calculator();

    // Main public interface
    // Converts infix expression to RPN and calculates the result
    double calculate(const char* infix_expression);

    // Convert infix to RPN using shunting yard algorithm
    void infixToRPN(const char* infix_expression);

    // Evaluate RPN expression stored in tokens array
    double evaluateRPN();

    // Get the number of tokens
    int getTokenCount() const { return token_count; }

    // Get token at index
    Token getToken(int index) const { 
        if (index >= 0 && index < token_count) return tokens[index];
        return Token();
    }
};

#endif // __CALCULATOR__
