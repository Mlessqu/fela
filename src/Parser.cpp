#include "Parser.h++"
#define FMT_HEADER_ONLY
#include <iostream>
#include <fmt/format.h>

#include "SemanticChecker.h++"

namespace fela
{
    const Token& Parser::get_current_token()
    {
        return tokens_[cursor_];
    }


    bool Parser::is_type(TokenType _type)
    {
        return get_current_token().type_ == _type;
    }


    bool Parser::is_not_type(TokenType _type)
    {
        return !is_type(_type);
    }


    const Token Parser::consume_token()
    {
        const Token& token = tokens_[cursor_];
        if (token.type_ != TokenType::eof)
        {
            cursor_++;
        }
        return token;
    }


    bool Parser::expect_and_consume(TokenType _expected_type, std::string _error_message)
    {
        if (is_type(_expected_type))
        {
            consume_token();
            return true;
        }
        std::cout << _error_message;
        return false;
    }


    std::string Parser::expected_diff_symbol_error(std::string_view _expected)
    {
        const Token& tok = get_current_token();
        return fmt::format("At line {}, col {} expected '{}' but got '{}'", tok.line_, tok.column_, _expected,
                           tok.payload_);
    }


    Parser::Parser(std::vector<Token> _tokens) : tokens_(std::move(_tokens))
    {
    }


    std::unique_ptr<AstProgram> Parser::parse_program()
    {
        std::unique_ptr<AstProgram> program_tree = std::make_unique<AstProgram>();

        while (is_not_type(TokenType::eof))
        {
            if (is_type_specifier_keyword())
            {
                std::unique_ptr<AstBase> node;
                if (look_ahead(2) == TokenType::open_group)
                {
                    node = parse_function();
                }
                else
                {
                    node = parse_variable_declaration_instruction();
                }
                program_tree->nodes_.push_back(std::move(node));
            }
            else
            {
                auto invalid_token = consume_token();
                return nullptr;
            }
        }
        return program_tree;
    }


    std::unique_ptr<AstInstruction> Parser::parse_instruction()
    {
        if (is_type(TokenType::return_keyword))
        {
            return parse_return_instruction();
        }
        else if (is_type(TokenType::if_keyword))
        {
            return parse_if_instruction();
        }
        else if (is_type(TokenType::while_keyword))
        {
            return parse_while_instruction();
        }
        else if (is_type(TokenType::open_scope))
        {
            return parse_block_instruction();
        }
        else if (is_type(TokenType::type_int) || is_type(TokenType::type_bool))
        {
            return parse_variable_declaration_instruction();
        }
        else if (is_type(TokenType::identifier) && look_ahead(1) == TokenType::assign)
        {
            return parse_assign_instruction();
        }
        else
        {
            return parse_primary_instruction();
        }
    }


    std::unique_ptr<AstInstruction> Parser::parse_return_instruction()
    {
        auto ret_keyword = consume_token();
        auto return_value = std::make_unique<AstExpression>();
        if (is_not_type(TokenType::semi))
        {
            return_value = parse_expression();
        }
        expect_and_consume(TokenType::semi, expected_diff_symbol_error(";"));
        auto return_node = std::make_unique<AstReturnInstruction>();
        return_node->expression_ = std::move(return_value);
        return return_node;
    }


    std::unique_ptr<AstInstruction> Parser::parse_while_instruction()
    {
        consume_token(); //while while(condition) { instructions}
        expect_and_consume(TokenType::open_group, "Invalid syntax, expected '(' after while");
        auto while_condition = parse_expression();
        expect_and_consume(TokenType::close_group, expected_diff_symbol_error(")"));
        auto while_body = parse_instruction();
        auto node_instruction = std::make_unique<AstWhileInstruction>();
        node_instruction->body_ = std::move(while_body);
        node_instruction->condition_ = std::move(while_condition);
        return node_instruction;
    }


    std::unique_ptr<AstInstruction> Parser::parse_if_instruction()
    {
        consume_token();
        expect_and_consume(TokenType::open_group, "Invalid syntax, expected '(' after if");
        auto if_condition = parse_expression();
        expect_and_consume(TokenType::close_group, expected_diff_symbol_error(")"));
        auto then_body = parse_instruction();
        std::unique_ptr<AstInstruction> optional_else = nullptr;
        if (is_type(TokenType::else_keyword))
        {
            consume_token();
            optional_else = parse_instruction();
        }
        auto return_node = std::make_unique<AstIfInstruction>();
        return_node->condition_ = std::move(if_condition);
        return_node->then_ = std::move(then_body);
        return_node->else_branch_ = std::move(optional_else);
        return return_node;
    }


    std::unique_ptr<AstInstruction> Parser::parse_variable_declaration_instruction()
    {
        auto declare_var_node = std::make_unique<AstVariableDeclaration>();

        auto token_type = consume_token(); //type keyword

        if (token_type.payload_ == "bool")
        {
            declare_var_node->type_ = DataType::bool_type;
        }
        else if (token_type.payload_ == "int")
        {
            declare_var_node->type_ = DataType::int_type;
        }
        else
        {
            declare_var_node->type_ = DataType::void_type;
        }
        auto id_token = get_current_token();
        expect_and_consume(TokenType::identifier, "Unexpected identifier syntax\n");
        declare_var_node->identifier_ = id_token.payload_;
        if (is_type(TokenType::assign))
        {
            consume_token();
            declare_var_node->init_value_ = parse_expression();
            expect_and_consume(TokenType::semi, expected_diff_symbol_error(";"));
            return declare_var_node;
        }
        expect_and_consume(TokenType::semi, expected_diff_symbol_error(";"));
        declare_var_node->init_value_ = nullptr;
        return declare_var_node;
    }


    std::unique_ptr<AstInstruction> Parser::parse_block_instruction()
    {
        auto ret_node = std::make_unique<AstBlockInstruction>();

        consume_token();
        while (is_not_type(TokenType::close_scope) && is_not_type(TokenType::eof))
        {
            ret_node->body_.push_back(std::move(parse_instruction()));
        }
        expect_and_consume(TokenType::close_scope, expected_diff_symbol_error("}"));
        return ret_node;
    }


    std::unique_ptr<AstInstruction> Parser::parse_assign_instruction()
    {
        Token id_token = consume_token(); //identifier

        Token op = consume_token(); // equal sign
        auto value = parse_expression(); // expression
        if (!expect_and_consume(TokenType::semi, expected_diff_symbol_error(";")))
        {
            return nullptr;
        }
        auto return_node = std::make_unique<AstAssignInstruction>();
        return_node->identifier_ = id_token.payload_;
        return_node->rhs_ = std::move(value);
        return return_node;
    }


    std::unique_ptr<AstInstruction> Parser::parse_primary_instruction()
    {
        auto ret_node = std::make_unique<AstPrimaryInstruction>();
        ret_node->expression_ = parse_expression();
        if (!expect_and_consume(TokenType::semi, expected_diff_symbol_error(";")))
        {
            return nullptr;
        }
        return ret_node;
    }


    std::unique_ptr<AstExpression> Parser::parse_expression()
    {
        return parse_boolean_logic_or_expression();
    }


    TokenType Parser::look_ahead(size_t _offset)
    {
        const size_t assumed_size = cursor_ + _offset;
        if (assumed_size >= tokens_.size())
        {
            return TokenType::eof;
        }
        return tokens_[cursor_ + _offset].type_;
    }


    std::unique_ptr<AstExpression> Parser::parse_grouped_expression()
    {
        if (!expect_and_consume(TokenType::open_group, expected_diff_symbol_error("(")))
        {
            return nullptr;
        }
        auto ret_node = parse_expression();
        if (!expect_and_consume(TokenType::close_group, expected_diff_symbol_error(")")))
        {
            return nullptr;
        }
        return ret_node;
    }


    std::unique_ptr<AstExpression> Parser::parse_primary_expression()
    {
        if (is_type(TokenType::identifier))
        {
            TokenType ahead = look_ahead();
            if (ahead == TokenType::open_group) // (
            {
                auto ret_node = parse_function_call();
                return ret_node;
            }
            else
            {
                Token id_token = consume_token(); // identifier
                auto ret_node = std::make_unique<AstVariableExpression>();
                ret_node->identifier_ = id_token.payload_;
                return ret_node;
            }
        }
        if (is_type(TokenType::integer_literal) || is_type(TokenType::false_boolean) ||
            is_type(TokenType::true_boolean))
        {
            return parse_literal_expression();
        }
        if (is_type(TokenType::open_group))
        {
            return parse_grouped_expression();
        }
        return nullptr;
    }


    std::unique_ptr<AstExpression> Parser::parse_literal_expression()
    {
        //literals we have right boolean and integer
        auto literal_expression = std::make_unique<AstLiteralExpression>();
        Token literal = consume_token();
        if (literal.type_ == TokenType::integer_literal)
        {
            int integer = std::stoi(std::string(literal.payload_));
            literal_expression->resolved_type_ = DataType::int_type;
            literal_expression->value_ = integer;
            return literal_expression;
        }
        if (literal.type_ == TokenType::true_boolean)
        {
            literal_expression->resolved_type_ = DataType::bool_type;
            literal_expression->value_ = true;
            return literal_expression;
        }
        if (literal.type_ == TokenType::false_boolean)
        {
            literal_expression->resolved_type_ = DataType::bool_type;
            literal_expression->value_ = false;
            return literal_expression;
        }
        return nullptr;
    }


    std::unique_ptr<AstExpression> Parser::parse_unary_expression()
    {
        if (is_type(TokenType::plus) || is_type(TokenType::minus) || is_type(TokenType::negation_op))
        {
            auto ret_node = std::make_unique<AstUnaryExpression>();
            Token token = consume_token();
            ret_node->operator_ = token;
            ret_node->rhs_ = parse_unary_expression();

            return ret_node;
        }
        return parse_primary_expression();
    }


    std::unique_ptr<AstExpression> Parser::parse_multiplying_expression()
    {
        auto lhs = parse_unary_expression();
        while(is_type(TokenType::multiply_op) || is_type(TokenType::divide_op))
        {
            Token operator_token = consume_token();
            auto rhs = parse_unary_expression();

            auto ret_node = std::make_unique<AstBinaryExpression>();
            ret_node->operator_ = operator_token;
            ret_node->rhs_ = std::move(rhs);
            ret_node->lhs_ = std::move(lhs);
            lhs = std::move(ret_node);
        }
        return lhs;
    }
    std::unique_ptr<AstExpression> Parser::parse_additive_expression()
    {
        auto lhs = parse_multiplying_expression();
        while (is_type(TokenType::minus) || is_type(TokenType::plus))
        {
            Token operator_token = consume_token();
            auto rhs = parse_multiplying_expression();

            auto ret_node = std::make_unique<AstBinaryExpression>();
            ret_node->operator_ = operator_token;
            ret_node->lhs_ = std::move(lhs);
            ret_node->rhs_ = std::move(rhs);
            lhs = std::move(ret_node);
        }
        return lhs;
    }


    std::unique_ptr<AstExpression> Parser::parse_relational_expression()
    {
        auto lhs = parse_additive_expression();
        while (is_type(TokenType::smaller_op) || is_type(TokenType::greater_op))
        {
            Token operator_token = consume_token();
            auto rhs = parse_additive_expression();

            auto ret_node = std::make_unique<AstBinaryExpression>();
            ret_node->operator_ = operator_token;
            ret_node->lhs_ = std::move(lhs);
            ret_node->rhs_ = std::move(rhs);
            lhs = std::move(ret_node);
        }
        return lhs;
    }


    std::unique_ptr<AstExpression> Parser::parse_equality_expression()
    {
        auto lhs = parse_relational_expression();
        while (is_type(TokenType::equal_op) || is_type(TokenType::not_equal_op))
        {
            Token operator_token = consume_token();
            auto rhs = parse_relational_expression();

            auto ret_node = std::make_unique<AstBinaryExpression>();
            ret_node->operator_ = operator_token;
            ret_node->lhs_ = std::move(lhs);
            ret_node->rhs_ = std::move(rhs);
            lhs = std::move(ret_node);
        }
        return lhs;
    }


    std::unique_ptr<AstExpression> Parser::parse_boolean_logic_and_expression()
    {
        auto lhs = parse_equality_expression();
        while (is_type(TokenType::and_op))
        {
            Token operator_token = consume_token();
            auto rhs = parse_equality_expression();

            auto ret_node = std::make_unique<AstBinaryExpression>();
            ret_node->operator_ = operator_token;
            ret_node->lhs_ = std::move(lhs);
            ret_node->rhs_ = std::move(rhs);
            lhs = std::move(ret_node);
        }
        return lhs;
    }


    std::unique_ptr<AstExpression> Parser::parse_boolean_logic_or_expression()
    {
        auto lhs = parse_boolean_logic_and_expression();
        while (is_type(TokenType::or_op))
        {
            Token operator_token = consume_token();
            auto rhs = parse_boolean_logic_and_expression();

            auto ret_node = std::make_unique<AstBinaryExpression>();
            ret_node->operator_ = operator_token;
            ret_node->lhs_ = std::move(lhs);
            ret_node->rhs_ = std::move(rhs);
            lhs = std::move(ret_node);
        }
        return lhs;
    }


    DataType Parser::token_type_to_data_type(TokenType _type)
    {
        switch (_type)
        {
        case TokenType::type_int: return DataType::int_type;
        case TokenType::type_bool: return DataType::bool_type;
        case TokenType::type_void: return DataType::void_type;
        default: return DataType::unresolved_type;
        }
    }


    std::vector<VariableSymbol> Parser::parse_param_list()
    {
        std::vector<VariableSymbol> param_list;
        expect_and_consume(TokenType::open_group, expected_diff_symbol_error("("));

        // empty param list ()
        if (is_type(TokenType::close_group))
        {
            consume_token();
            return param_list;
        }

        while (true)
        {
            if (!is_type_specifier_keyword())
            {
                break;
            }
            const Token type_token = consume_token();
            const Token name_token = get_current_token();
            expect_and_consume(TokenType::identifier, expected_diff_symbol_error("identifier"));

            param_list.push_back(VariableSymbol{
                .name_ = std::string(name_token.payload_),
                .type_ = token_type_to_data_type(type_token.type_)
            });

            if (is_type(TokenType::coma))
            {
                consume_token();
            }
            else
            {
                break;
            }
        }

        expect_and_consume(TokenType::close_group, expected_diff_symbol_error(")"));
        return param_list;
    }


    std::unique_ptr<AstFunction> Parser::parse_function()
    {
        Token return_type = consume_token();
        Token identifier_token = get_current_token();
        expect_and_consume(TokenType::identifier, "Expected function name");
        std::string function_name = std::string(identifier_token.payload_);
        std::vector<VariableSymbol> params = parse_param_list();

        if (is_type(TokenType::open_scope))
        {
            auto function_def = std::make_unique<AstFunctionDefinition>();
            function_def->identifier_ = function_name;
            function_def->return_type_ = token_type_to_data_type(return_type.type_);
            function_def->parameters_ = std::move(params);
            auto block = parse_block_instruction();
            function_def->body_ = std::unique_ptr<AstBlockInstruction>(static_cast<AstBlockInstruction*>(block.release()));
            return function_def;
        }
        else
        {
            expect_and_consume(TokenType::semi, expected_diff_symbol_error(";"));
            auto function_decl = std::make_unique<AstFunctionDeclaration>();
            function_decl->identifier_ = function_name;
            function_decl->return_type_ = token_type_to_data_type(return_type.type_);
            function_decl->parameters_ = std::move(params);
            return function_decl;
        }
    }


    std::vector<std::unique_ptr<AstExpression>> Parser::parse_argument_list()
    {
        std::vector<std::unique_ptr<AstExpression>> args;
        expect_and_consume(TokenType::open_group, expected_diff_symbol_error("("));
        if (is_type(TokenType::close_group)) //empty call
        {
            expect_and_consume(TokenType::close_group, expected_diff_symbol_error(")"));
            return args;
        }
        if (is_not_type(TokenType::close_group))
        {
            args.push_back(parse_expression());
        }
        while (is_not_type(TokenType::close_group))
        {
            expect_and_consume(TokenType::coma, expected_diff_symbol_error(","));
            args.push_back(parse_expression());
        }
        expect_and_consume(TokenType::close_group, expected_diff_symbol_error(")"));
        return args;
    }


    bool Parser::is_type_specifier_keyword()
    {
        if (is_type(TokenType::type_bool)) return true;
        if (is_type(TokenType::type_int)) return true;
        if (is_type(TokenType::type_void)) return true;
        return false;
    }


    std::unique_ptr<AstExpression> Parser::parse_function_call()
    {
        Token id_token = consume_token();
        std::string identifier{id_token.payload_};
        auto args = parse_argument_list();
        auto ret_node = std::make_unique<AstFunctionCall>();
        ret_node->identifier_ = identifier;
        ret_node->arguments_ = std::move(args);
        return ret_node;
    }


    //implementation details here
} // fela
