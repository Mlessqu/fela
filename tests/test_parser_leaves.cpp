#include "ParserFixture.h++"

TEST_SUITE("parser: leaves")
{
    TEST_CASE_FIXTURE(ParserFixture, "leaf node: integer literal")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with a identifier, with init value:\n"
            "    Literal expression node:\n"
            "    Value:42\n";

        CHECK(dump("int a = 42;") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "leaf node: boolean literals")
    {
        const std::string expected_true =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with flag identifier, with init value:\n"
            "    Literal expression node:\n"
            "    Value:true\n";

        CHECK(dump("bool flag = true;") == strip_indent(expected_true));

        const std::string expected_false =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with flag identifier, with init value:\n"
            "    Literal expression node:\n"
            "    Value:false\n";

        CHECK(dump("bool flag = false;") == strip_indent(expected_false));
    }

    TEST_CASE_FIXTURE(ParserFixture, "leaf node: variable declarations without init value")
    {
        const std::string expected_int =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with x identifier, without init value\n";

        CHECK(dump("int x;") == strip_indent(expected_int));

        const std::string expected_bool =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with flag identifier, without init value\n";

        CHECK(dump("bool flag;") == strip_indent(expected_bool));

        const std::string expected_multiple =
            "Ast Program node with 2 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with a identifier, without init value\n"
            "Node number 2:\n"
            "  Ast variable declaration node with b identifier, without init value\n";

        CHECK(dump("int a; bool b;") == strip_indent(expected_multiple));
    }

    TEST_CASE_FIXTURE(ParserFixture, "leaf node: variable expression")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Ast variable declaration node with x identifier, with init value:\n"
            "    Ast variable expression node:\n"
            "    identifier: y\n";

        CHECK(dump("int x = y;") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "leaf node: function declaration without params")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function declaration node:\n"
            "  identifier: foo\n"
            "  return type: void\n"
            "  with 0 params:\n";

        CHECK(dump("void foo();") == strip_indent(expected));
    }
}
