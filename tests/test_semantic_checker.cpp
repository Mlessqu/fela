#include "SemanticFixture.h++"

TEST_SUITE("semantic checker")
{
    TEST_CASE_FIXTURE(SemanticFixture, "variable declaration: valid types")
    {
        CHECK(check("int a = 10; bool b = true;"));
    }

    TEST_CASE_FIXTURE(SemanticFixture, "variable declaration: type mismatch on init")
    {
        CHECK_FALSE(check("int a = true;"));
        CHECK(has_error_containing("types must match"));
    }

    TEST_CASE_FIXTURE(SemanticFixture, "variable declaration: redefinition")
    {
        CHECK_FALSE(check("int a = 1; int a = 2;"));
        CHECK(has_error_containing("Redefinition"));
    }

    TEST_CASE_FIXTURE(SemanticFixture, "assignment: valid and type mismatch")
    {
        CHECK(check("int a = 1; void main() { a = 2; }"));
        CHECK_FALSE(check("int a = 1; void main() { a = false; }"));

        CHECK(check("void main() { int a = 1; a = 2; }"));
        CHECK_FALSE(check("void main() { int a = 1; a = false; }"));
    }

    TEST_CASE_FIXTURE(SemanticFixture, "assignment: undeclared variable")
    {
        CHECK_FALSE(check("void main() { x = 42; }"));
        CHECK(has_error_containing("Unknown identifier"));
    }

    TEST_CASE_FIXTURE(SemanticFixture, "function declaration: valid and duplicate params")
    {
        CHECK(check("int add(int a, int b);"));
        CHECK_FALSE(check("int foo(int x, int x);"));
        CHECK(has_error_containing("Duplicate parameter"));
    }

    TEST_CASE_FIXTURE(SemanticFixture, "function declaration: redefinition")
    {
        CHECK_FALSE(check("int foo(); int foo();"));
        CHECK(has_error_containing("Redefinition"));
    }

    TEST_CASE_FIXTURE(SemanticFixture, "function call: valid arguments")
    {
        CHECK(check("int add(int a, int b); int res = add(1, 2);"));
    }

    TEST_CASE_FIXTURE(SemanticFixture, "function call: wrong argument count")
    {
        CHECK_FALSE(check("int add(int a, int b); add(1);"));
        CHECK(has_error_containing("expects 2 arguments"));
    }

    TEST_CASE_FIXTURE(SemanticFixture, "function call: argument type mismatch")
    {
        CHECK_FALSE(check("int add(int a, int b); add(1, true);"));
        CHECK(has_error_containing("type mismatch"));
    }
}
