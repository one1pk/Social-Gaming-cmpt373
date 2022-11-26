//
// Created by DELL on 2022/11/14.
//
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../lib/game/include/list.h"
using namespace std;
using namespace testing;
using ::testing::AtLeast;

/**
Map elements
*/

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
    EXPECT_EQ(test_vector_ptr->getVector(), test_vector);}


// // TODO: The following test is failing

// TEST(ElementTest, getSubListTest) {
//     ElementVector vector_of_maps;

//     ElementSptr first_string = make_shared<Element<std::string>>("First string");
//     map<string, shared_ptr<ListElement>> first_map {{"test", first_string}};
//     auto first_map_ptr = make_shared<map<string, shared_ptr<ListElement>>>(first_map);
    
//     ElementSptr second_string = make_shared<Element<std::string>>("Second string");
//     map<string, shared_ptr<ListElement>> second_map {{"test", second_string}};
//     auto second_map_ptr = make_shared<map<string, shared_ptr<ListElement>>>(second_map);

//     vector_of_maps.push_back(first_map_ptr);
//     vector_of_maps.push_back(second_map_ptr);

//     ElementVector res_vector;
//     res_vector.push_back(first_string);
//     res_vector.push_back(second_string);

//     EXPECT_EQ(vector_of_maps.getSubList("test"), res_vector);
// }