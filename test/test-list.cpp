//
// Created by DELL on 2022/11/14.
//
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../lib/game/include/list.h"
using namespace std;
using namespace testing;
using ::testing::AtLeast;

TEST (ElementTest, getMapElementTest) {
    ElementSptr test_element_string = make_shared<Element<std::string>>("This is a test");
    map<string, shared_ptr<ListElement>> map_sample {{"first", test_element_string}};
    ElementMap temp_map = map_sample;
    ElementSptr test_map = make_shared<Element<ElementMap>>(temp_map);
    EXPECT_EQ(test_map->getMapElement("first")->getString(), "This is a test");
}

TEST (ElementTest, setMapElementTest) {
    ElementSptr test_element_string = make_shared<Element<string>>("This is a test");
    map<string, shared_ptr<ListElement>> map_sample {{"first", test_element_string}};
    ElementMap temp_map = map_sample;
    ElementSptr test_map = make_shared<Element<ElementMap>>(temp_map);
    test_map->setMapElement(
        "first", make_shared<Element<string>>("after_set")
    );
    EXPECT_EQ(test_map->getMapElement("first")->getString(), "after_set");
}

TEST (ElementTest, removeMapElementTest) {
    ElementSptr test_element_string = make_shared<Element<string>>("This is a test");
    map<string, shared_ptr<ListElement>> map_sample {{"first", test_element_string}};
    ElementMap temp_map = map_sample;
    ElementSptr test_map = make_shared<Element<ElementMap>>(temp_map);
    test_map->removeMapElement("first");
    EXPECT_EQ(test_map->getMapElement("first"), nullptr);
}

TEST (ElementTest, getIntTest) {
    ElementSptr test_element_int = make_shared<Element<int>>(99);
    EXPECT_EQ(test_element_int->getInt(), 99);
}

TEST(ElementTest, setIntTest) {
    ElementSptr test_element_int = make_shared<Element<int>>(99);
    test_element_int->setInt(101);
    EXPECT_EQ(test_element_int->getInt(), 101);
}

TEST(ElementTest, addIntTest) {
    ElementSptr test_element_int = make_shared<Element<int>>(99);
    test_element_int->addInt(100);
    EXPECT_EQ(test_element_int->getInt(), 199);
}

// TEST(ElementTest, )