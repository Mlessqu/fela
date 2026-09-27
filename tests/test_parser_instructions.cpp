#include "ParserFixture.h++"

TEST_SUITE("parser: instructions")
{
    TEST_CASE_FIXTURE(ParserFixture, "function definition: empty and with params")
    {
        const std::string expected_empty =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function definition node:\n"
            "  identifier: foo\n"
            "  return type: void\n"
            "  with 0 params:\n"
            "  Body:\n"
            "    Ast block instruction with 0 instructions:\n";

        CHECK(dump("void foo() {}") == strip_indent(expected_empty));

        const std::string expected_params =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function definition node:\n"
            "  identifier: add\n"
            "  return type: int\n"
            "  with 2 params:\n"
            "  Param: a (int)\n"
            "  Param: b (int)\n"
            "  Body:\n"
            "    Ast block instruction with 0 instructions:\n";

        CHECK(dump("int add(int a, int b) {}") == strip_indent(expected_params));
    }

    TEST_CASE_FIXTURE(ParserFixture, "instruction: assign")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function definition node:\n"
            "  identifier: test\n"
            "  return type: void\n"
            "  with 0 params:\n"
            "  Body:\n"
            "    Ast block instruction with 1 instructions:\n"
            "      Assign instruction node with x identifier:\n"
            "        Literal expression node:\n"
            "        Value:42\n";

        CHECK(dump("void test() { x = 42; }") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "instruction: return")
    {
        const std::string expected_void =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function definition node:\n"
            "  identifier: test\n"
            "  return type: void\n"
            "  with 0 params:\n"
            "  Body:\n"
            "    Ast block instruction with 1 instructions:\n"
            "      Ast return instruction node:\n";

        CHECK(dump("void test() { return; }") == strip_indent(expected_void));

        const std::string expected_val =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function definition node:\n"
            "  identifier: test\n"
            "  return type: int\n"
            "  with 0 params:\n"
            "  Body:\n"
            "    Ast block instruction with 1 instructions:\n"
            "      Ast return instruction node:\n"
            "        Literal expression node:\n"
            "        Value:42\n";

        CHECK(dump("int test() { return 42; }") == strip_indent(expected_val));
    }

    TEST_CASE_FIXTURE(ParserFixture, "instruction: if without else and with else")
    {
        const std::string expected_no_else =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function definition node:\n"
            "  identifier: test\n"
            "  return type: void\n"
            "  with 0 params:\n"
            "  Body:\n"
            "    Ast block instruction with 1 instructions:\n"
            "      If instruction node\n"
            "      with condition:\n"
            "        Ast variable expression node:\n"
            "          identifier: flag\n"
            "      with body:\n"
            "        Ast block instruction with 1 instructions:\n"
            "          Ast return instruction node:\n"
            "      without else\n";

        CHECK(dump("void test() { if (flag) { return; } }") == strip_indent(expected_no_else));

        const std::string expected_with_else =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function definition node:\n"
            "  identifier: test\n"
            "  return type: void\n"
            "  with 0 params:\n"
            "  Body:\n"
            "    Ast block instruction with 1 instructions:\n"
            "      If instruction node\n"
            "      with condition:\n"
            "        Ast variable expression node:\n"
            "          identifier: flag\n"
            "      with body:\n"
            "        Ast block instruction with 1 instructions:\n"
            "          Assign instruction node with x identifier:\n"
            "            Literal expression node:\n"
            "            Value:1\n"
            "      with else:\n"
            "        Ast block instruction with 1 instructions:\n"
            "          Assign instruction node with x identifier:\n"
            "            Literal expression node:\n"
            "            Value:2\n";

        CHECK(dump("void test() { if (flag) { x = 1; } else { x = 2; } }") == strip_indent(expected_with_else));
    }

    TEST_CASE_FIXTURE(ParserFixture, "instruction: while")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function definition node:\n"
            "  identifier: test\n"
            "  return type: void\n"
            "  with 0 params:\n"
            "  Body:\n"
            "    Ast block instruction with 1 instructions:\n"
            "      Ast while instruction node\n"
            "      Ast while instruction condition:\n"
            "        Ast variable expression node:\n"
            "          identifier: flag\n"
            "      Ast while instruction body:\n"
            "        Ast block instruction with 1 instructions:\n"
            "          Assign instruction node with x identifier:\n"
            "            Literal expression node:\n"
            "            Value:1\n";

        CHECK(dump("void test() { while (flag) { x = 1; } }") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "instruction: primary expression statement")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function definition node:\n"
            "  identifier: test\n"
            "  return type: void\n"
            "  with 0 params:\n"
            "  Body:\n"
            "    Ast block instruction with 1 instructions:\n"
            "      Ast primary instruction node:\n"
            "        Ast function call node:\n"
            "        identifier: foo\n"
            "        return type: unresolved_type\n"
            "        with 0 arguments:\n";

        CHECK(dump("void test() { foo(); }") == strip_indent(expected));
    }

    TEST_CASE_FIXTURE(ParserFixture, "instruction: local variable declaration")
    {
        const std::string expected =
            "Ast Program node with 1 nodes\n"
            "Node number 1:\n"
            "  Function definition node:\n"
            "  identifier: test\n"
            "  return type: void\n"
            "  with 0 params:\n"
            "  Body:\n"
            "    Ast block instruction with 1 instructions:\n"
            "      Ast variable declaration node with x identifier, with init value:\n"
            "        Literal expression node:\n"
            "        Value:10\n";

        CHECK(dump("void test() { int x = 10; }") == strip_indent(expected));
    }
}
