#include "ParserFixture.h++"

TEST_SUITE("parser: expressions")
{
    TEST_CASE_FIXTURE(ParserFixture, "unary expression: negation and not")
    {
        const std::string expected_minus =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with x identifier, with init value:\n"
            "    Unary expression node:\n"
            "    Unary operator:-\n"
            "    Rhs:\n"
            "      Literal expression node:\n"
            "      Value:42\n";

        CHECK(dump("int x = -42;") == strip_indent(expected_minus));

        const std::string expected_not =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with flag identifier, with init value:\n"
            "    Unary expression node:\n"
            "    Unary operator:!\n"
            "    Rhs:\n"
            "      Literal expression node:\n"
            "      Value:true\n";

        CHECK(dump("bool flag = !true;") == strip_indent(expected_not));
    }

    TEST_CASE_FIXTURE(ParserFixture, "binary expression: arithmetic")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with x identifier, with init value:\n"
            "    Ast binary expression node:\n"
            "    lhs:\n"
            "      Literal expression node:\n"
            "      Value:1\n"
            "    Binary operator:+\n"
            "    Rhs:\n"
            "      Literal expression node:\n"
            "      Value:2\n";

        CHECK(dump("int x = 1 + 2;") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "binary expression: operator precedence")
    {
        // 1 + 2 * 3 => (+ 1 (* 2 3))
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with x identifier, with init value:\n"
            "    Ast binary expression node:\n"
            "    lhs:\n"
            "      Literal expression node:\n"
            "      Value:1\n"
            "    Binary operator:+\n"
            "    Rhs:\n"
            "      Ast binary expression node:\n"
            "      lhs:\n"
            "        Literal expression node:\n"
            "        Value:2\n"
            "      Binary operator:*\n"
            "      Rhs:\n"
            "        Literal expression node:\n"
            "        Value:3\n";

        CHECK(dump("int x = 1 + 2 * 3;") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "grouped expression: overrides precedence")
    {
        // (1 + 2) * 3 => (* (+ 1 2) 3)
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with x identifier, with init value:\n"
            "    Ast binary expression node:\n"
            "    lhs:\n"
            "      Ast binary expression node:\n"
            "      lhs:\n"
            "        Literal expression node:\n"
            "        Value:1\n"
            "      Binary operator:+\n"
            "      Rhs:\n"
            "        Literal expression node:\n"
            "        Value:2\n"
            "    Binary operator:*\n"
            "    Rhs:\n"
            "      Literal expression node:\n"
            "      Value:3\n";

        CHECK(dump("int x = (1 + 2) * 3;") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "binary expression: relational and equality")
    {
        const std::string expected_lt =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with r identifier, with init value:\n"
            "    Ast binary expression node:\n"
            "    lhs:\n"
            "      Ast variable expression node:\n"
            "        identifier: a\n"
            "    Binary operator:<\n"
            "    Rhs:\n"
            "      Ast variable expression node:\n"
            "        identifier: b\n";

        CHECK(dump("bool r = a < b;") == strip_indent(expected_lt));

        const std::string expected_eq =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with r identifier, with init value:\n"
            "    Ast binary expression node:\n"
            "    lhs:\n"
            "      Ast variable expression node:\n"
            "        identifier: a\n"
            "    Binary operator:==\n"
            "    Rhs:\n"
            "      Ast variable expression node:\n"
            "        identifier: b\n";

        CHECK(dump("bool r = a == b;") == strip_indent(expected_eq));
    }

    TEST_CASE_FIXTURE(ParserFixture, "binary expression: logical and / or")
    {
        // a && b || c => (|| (&& a b) c)
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with r identifier, with init value:\n"
            "    Ast binary expression node:\n"
            "    lhs:\n"
            "      Ast binary expression node:\n"
            "      lhs:\n"
            "        Ast variable expression node:\n"
            "          identifier: a\n"
            "      Binary operator:&&\n"
            "      Rhs:\n"
            "        Ast variable expression node:\n"
            "          identifier: b\n"
            "    Binary operator:||\n"
            "    Rhs:\n"
            "      Ast variable expression node:\n"
            "        identifier: c\n";

        CHECK(dump("bool r = a && b || c;") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "function call expression")
    {
        const std::string expected_no_args =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with x identifier, with init value:\n"
            "    Ast function call node:\n"
            "    identifier: foo\n"
            "    return type: unresolved_type\n"
            "    with 0 arguments:\n";

        CHECK(dump("int x = foo();") == strip_indent(expected_no_args));

        const std::string expected_with_args =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with x identifier, with init value:\n"
            "    Ast function call node:\n"
            "    identifier: add\n"
            "    return type: unresolved_type\n"
            "    with 2 arguments:\n"
            "      Literal expression node:\n"
            "      Value:1\n"
            "      Literal expression node:\n"
            "      Value:2\n";

        CHECK(dump("int x = add(1, 2);") == strip_indent(expected_with_args));
    }
}
