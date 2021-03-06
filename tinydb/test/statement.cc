/*
   Copyright 2021 Simon (psyomn) Symeonidis

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "tinydb/schema.h"
#include "tinydb/statement.h"

#include <optional>

const psy::tinydb::Error kDefaultError{"should have error"};

TEST(statement, invalid_statements) {
  using namespace psy;
  {
    psy::tinydb::Schema s;
    const auto stat = tinydb::Statement("nope nope nope", s);
    EXPECT_EQ(stat.GetStatementType(), tinydb::Statement::Type::Invalid);
    EXPECT_TRUE(stat.GetState().has_value());
    EXPECT_EQ(stat.GetState().value_or(kDefaultError).GetMessage(),
              "bad starter identifier");
  }
  {
    psy::tinydb::Schema s;
    const auto stat = tinydb::Statement("", s);
    EXPECT_EQ(stat.GetStatementType(), tinydb::Statement::Type::Invalid);
    EXPECT_TRUE(stat.GetState().has_value());
    EXPECT_EQ(stat.GetState().value_or(kDefaultError).GetMessage(),
              "empty query");
  }
}

TEST(statement, select_statement_correct_type) {
  psy::tinydb::Schema s;
  using namespace psy;
  const auto stat = tinydb::Statement("select * from test_table", s);
  EXPECT_EQ(stat.GetStatementType(), tinydb::Statement::Type::Select);
}

TEST(statement, insert_statement_correct_type) {
  psy::tinydb::Schema s;
  using namespace psy;
  const auto stat = tinydb::Statement("insert into test_table col1 col2 col3 values 1 2 3", s);
  EXPECT_EQ(stat.GetStatementType(), tinydb::Statement::Type::Insert);
}

TEST(statement, parse_column_size) {
  { // good
    const std::string s1 = "varchar(32)";
    const std::string s2 = "varchar(255)";
    const std::string s3 = "int";

    const auto res1 = psy::tinydb::ParseColumnSize(s1);
    ASSERT_TRUE(res1.has_value());
    EXPECT_EQ(res1.value(), 32);

    const auto res2 = psy::tinydb::ParseColumnSize(s2);
    ASSERT_TRUE(res2.has_value());
    EXPECT_EQ(res2.value(), 255);

    const auto res3 = psy::tinydb::ParseColumnSize(s3);
    ASSERT_TRUE(res3.has_value());
    EXPECT_EQ(res3.value(), 4);
  }

  { // bad
    const std::string s1 = "varchar(0)";
    const std::string s2 = "varchar()";
    const std::string s3 = "varchar";

    const auto res1 = psy::tinydb::ParseColumnSize(s1);
    ASSERT_FALSE(res1.has_value());

    const auto res2 = psy::tinydb::ParseColumnSize(s2);
    ASSERT_FALSE(res2.has_value());

    const auto res3 = psy::tinydb::ParseColumnSize(s3);
    ASSERT_FALSE(res3.has_value());
  }
}

TEST(statement, parse_column_type) {
  {
    const std::string s1 = "int";
    const auto res1 = psy::tinydb::ParseColumnType(s1);
    ASSERT_EQ(res1.value(), psy::tinydb::ColumnType::Integer);

    const std::string s2 = "varchar(32)";
    const auto res2 = psy::tinydb::ParseColumnType(s2);
    ASSERT_EQ(res2.value(), psy::tinydb::ColumnType::String);
  }
  {
    const std::string s1 = "nope";
    const auto res1 = psy::tinydb::ParseColumnType(s1);
    ASSERT_FALSE(res1.has_value());

    const std::string s2 = "nope(32)";
    const auto res2 = psy::tinydb::ParseColumnType(s2);
    ASSERT_FALSE(res2.has_value());
  }
}

TEST(statement, create_table) {
  psy::tinydb::Schema s;
  const std::string create_table =
    "create table people id int name varchar(32) email varchar(256)";

  psy::tinydb::Statement stat(create_table, s);
  EXPECT_FALSE(stat.GetState().has_value());
  EXPECT_EQ(stat.GetStatementType(), psy::tinydb::Statement::Type::Create);

  const auto names = s.GetTableNames();
  EXPECT_EQ(names, std::vector<std::string>{"people"});

  ASSERT_TRUE(s.FindTableByName("people").has_value());
  const auto columns = s.FindTableByName("people").value()->columns_;

  EXPECT_EQ(columns.size(), 3);

  EXPECT_EQ(columns[0].label_, "id");
  EXPECT_EQ(columns[0].size_, 4);
  EXPECT_EQ(columns[0].type_, psy::tinydb::ColumnType::Integer);

  EXPECT_EQ(columns[1].label_, "name");
  EXPECT_EQ(columns[1].size_, 32);
  EXPECT_EQ(columns[1].type_, psy::tinydb::ColumnType::String);

  EXPECT_EQ(columns[2].label_, "email");
  EXPECT_EQ(columns[2].size_, 256);
  EXPECT_EQ(columns[2].type_, psy::tinydb::ColumnType::String);
}

TEST(statement, unique_columns) {
  psy::tinydb::Schema s;
  const std::string dup_cols =
    "create table people name varchar(32) name varchar(32)";

  psy::tinydb::Statement stat(dup_cols, s);

  EXPECT_TRUE(stat.GetState().has_value());
  EXPECT_EQ(stat.GetState()
                .value_or(kDefaultError)
                .GetMessage(), "duplicate column names");
}

TEST(statement, parse_bad_select_statement) {
  psy::tinydb::Schema s;
  const auto stat1 = psy::tinydb::Statement("select *", s);
  const auto stat2 = psy::tinydb::Statement("select * from", s);

  EXPECT_EQ(stat1.GetState()
            .value_or(kDefaultError)
            .GetMessage(), "invalid select statement: expecting at least 4 parts");

  EXPECT_EQ(stat2.GetState()
            .value_or(kDefaultError)
            .GetMessage(), "invalid select statement: expecting at least 4 parts");
}

TEST(statement, parse_select_wildcard) {
  psy::tinydb::Schema s;
  const auto stat = psy::tinydb::Statement("select * from people", s);

  EXPECT_EQ(stat.GetStatementType(), psy::tinydb::Statement::Type::Select);
  EXPECT_EQ(stat.HasWildcardValues(), true);
}

TEST(statement, parse_select_names) {
  psy::tinydb::Schema s;
  const auto select_stat =
    psy::tinydb::Statement("select name email from people", s);

  EXPECT_EQ(select_stat.GetStatementType(), psy::tinydb::Statement::Type::Select);

  const std::vector<std::string> expected_values = { "name", "email" };
  EXPECT_EQ(select_stat.GetValues(), expected_values);

  ASSERT_TRUE(select_stat.GetTableName().has_value());
  EXPECT_EQ(select_stat.GetTableName().value(), "people");
}
