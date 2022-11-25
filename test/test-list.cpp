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

TEST(ElementTest, getVectorTest) {
    vector<std::shared_ptr<ListElement>> temp_vector;
    
    ElementSptr test_element_int = make_shared<Element<int>>(1);
    temp_vector.push_back(test_element_int);
    test_element_int = make_shared<Element<int>>(2);
    temp_vector.push_back(test_element_int);
    test_element_int = make_shared<Element<int>>(3);
    temp_vector.push_back(test_element_int);

    ElementVector test_vector = temp_vector;
    ElementSptr test_vector_ptr = make_shared<Element<ElementVector>>(test_vector);
    EXPECT_EQ(
        test_vector_ptr->getVector(), 
        // TODO: What to compare?
        // vector({make_shared<int>(1), make_shared<int>(2), make_shared<int>(3)})
    );
}