//
// Created by DELL on 2022/11/14.
//
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../lib/game/include/list.h"
using namespace std;
using namespace testing;
using ::testing::AtLeast;

TEST (ElementTest,setMapElementTest ){
    ElementSptr test_element_string = make_shared<Element<std::string>>("this is a test");
    ElementMap temp_map=map<std::string, shared_ptr<ListElement>>("first",test_element_string);
    ElementSptr test_map = make_shared<Element<ElementMap>>(temp_map);
    EXPECT_EQ(test_map->getMapElement("first")->getString() == "this is a test");
}
