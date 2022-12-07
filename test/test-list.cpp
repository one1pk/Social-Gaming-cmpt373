#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../lib/game/include/list.h"
using namespace std;
using namespace testing;
using ::testing::AtLeast;


//================================================================
// Type: MAP
//================================================================

TEST (ElementTest, getMapTest) {
    ElementMap my_map {{"seventy-seven", make_shared<Element<int>>(77)}};
    ElementSptr test_map = make_shared<Element<ElementMap>>(my_map);
    EXPECT_EQ(test_map->getMap(), my_map);  
}

TEST (ElementTest, getMapTypeErrorTest) {
    ElementMap my_map;
    ElementSptr test_element_string = make_shared<Element<std::string>>(std::string("This is a test"));
    EXPECT_EQ(test_element_string->getMap(), my_map);
}

TEST (ElementTest, getMapElementTest) {
    ElementSptr test_element_string = make_shared<Element<std::string>>(std::string("This is a test"));
    map<string, shared_ptr<ListElement>> map_sample {{"first", test_element_string}};
    ElementMap temp_map = map_sample;
    ElementSptr test_map = make_shared<Element<ElementMap>>(temp_map);
    EXPECT_EQ(test_map->getMapElement("first")->getString(), "This is a test");
}

TEST (ElementTest, getMapElementErrorTest) {
    ElementSptr test_element_string = make_shared<Element<std::string>>(std::string("This is a test"));
    EXPECT_EQ(test_element_string->getMapElement("first"), nullptr);
}

TEST (ElementTest, setMapElementTest) {
    ElementSptr test_element_string = make_shared<Element<std::string>>(std::string("This is a test"));
    map<string, shared_ptr<ListElement>> map_sample {{"first", test_element_string}};
    ElementMap temp_map = map_sample;
    ElementSptr test_map = make_shared<Element<ElementMap>>(temp_map);
    test_map->setMapElement(
        "first", make_shared<Element<std::string>>(std::string("after_set"))
    );
    EXPECT_EQ(test_map->getMapElement("first")->getString(), "after_set");
}

TEST (ElementTest, removeMapElementTest) {
    ElementSptr test_element_string = make_shared<Element<std::string>>(std::string("This is a test"));
    map<string, shared_ptr<ListElement>> map_sample {{"first", test_element_string}};
    ElementMap temp_map = map_sample;
    ElementSptr test_map = make_shared<Element<ElementMap>>(temp_map);
    test_map->removeMapElement("first");
    EXPECT_EQ(test_map->getMapElement("first"), nullptr);
}

TEST (ElementTest, getSizeMapTest) {
    ElementSptr test_element_string_1 = make_shared<Element<std::string>>(std::string("This is a test 1"));
    ElementSptr test_element_string_2 = make_shared<Element<std::string>>(std::string("This is a test 2"));
    ElementSptr test_element_string_3 = make_shared<Element<std::string>>(std::string("This is a test 3"));
    map<string, shared_ptr<ListElement>> map_sample {{"first", test_element_string_1},{"second",test_element_string_2 },{"third",test_element_string_3 }};
    ElementMap temp_map = map_sample;
    ElementSptr test_map = make_shared<Element<ElementMap>>(temp_map);
    EXPECT_EQ(test_map->getSize(), 3);
}

TEST(ElementTest, mapCloneTest) {
    ElementMap my_map {{"ninety-nine", make_shared<Element<int>>(99)}};
    ElementSptr my_map_ptr = make_shared<Element<ElementMap>>(my_map);
    ElementSptr my_map_cloned_ptr = my_map_ptr->clone();

    EXPECT_EQ(
        my_map_ptr->getMapElement("ninety-nine")->getInt(),
        my_map_cloned_ptr->getMapElement("ninety-nine")->getInt()
    );
}


//================================================================
// Type: INT
//================================================================

TEST (ElementTest, getIntTest) {
    ElementSptr test_element_int = make_shared<Element<int>>(99);
    EXPECT_EQ(test_element_int->getInt(), 99);
}

TEST (ElementTest, getIntTypeErrorTest) {
    ElementSptr test_element_string = make_shared<Element<std::string>>(std::string("This is a test"));
    EXPECT_EQ(test_element_string->getInt(), 0);
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

TEST(ElementTest, upfromTestBig) {
    ElementSptr test_element_int = make_shared<Element<int>>(99);
    ElementSptr big_start = test_element_int->upfrom(101);

    ElementVector big_res;
    EXPECT_EQ(big_start->getVector(), big_res);
}

TEST(ElementTest, upfromTestSmall) {
    ElementSptr test_element_int = make_shared<Element<int>>(99);
    ElementSptr small_start = test_element_int->upfrom(97);
    ElementVector small_vec = small_start->getVector();
    
    ElementVector small_res;
    small_res.push_back(make_shared<Element<int>>(97));
    small_res.push_back(make_shared<Element<int>>(98));
    small_res.push_back(make_shared<Element<int>>(99));

    EXPECT_EQ(small_vec[0]->getInt(), small_res[0]->getInt());
    EXPECT_EQ(small_vec[1]->getInt(), small_res[1]->getInt());
    EXPECT_EQ(small_vec[2]->getInt(), small_res[2]->getInt());
}

TEST(ElementTest, intCloneTest) {    
    ElementSptr test_element_int = make_shared<Element<int>>(7);
    ElementSptr int_cloned_ptr = test_element_int->clone();
    EXPECT_EQ(int_cloned_ptr->getInt(), 7);
}

TEST(ElementTest, getSizeIntTest) {
    ElementSptr test_element_int = make_shared<Element<int>>(7);
    EXPECT_EQ(test_element_int->getSize(), 0);
}

//================================================================
// Type: VECTOR
//================================================================

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
    EXPECT_EQ(test_vector_ptr->getVector(), test_vector);
}

TEST(ElementTest, getVectorTypeErrorTest) {
    ElementSptr test_element_int = make_shared<Element<int>>(7);
    ElementVector test_vector;
    EXPECT_EQ(test_element_int->getVector(), test_vector);
}

TEST(ElementTest, getSizeVectorTest) {
    vector<std::shared_ptr<ListElement>> temp_vector;
    
    ElementSptr test_element_int = make_shared<Element<int>>(1);
    temp_vector.push_back(test_element_int);
    test_element_int = make_shared<Element<int>>(2);
    temp_vector.push_back(test_element_int);
    test_element_int = make_shared<Element<int>>(3);
    temp_vector.push_back(test_element_int);

    ElementVector test_vector = temp_vector;
    ElementSptr test_vector_ptr = make_shared<Element<ElementVector>>(test_vector);
    EXPECT_EQ(test_vector_ptr->getSize(), 3);
}

TEST(ElementTest, getSubListTest) {
    ElementVector vector_of_maps;

    ElementSptr first_string = make_shared<Element<std::string>>(std::string("First string"));
    map<string, shared_ptr<ListElement>> first_map {{"test", first_string}};
    ElementSptr first_map_ptr = make_shared<Element<ElementMap>>(first_map);
    
    ElementSptr second_string = make_shared<Element<std::string>>(std::string("Second string"));
    map<string, shared_ptr<ListElement>> second_map {{"test", second_string}};
    ElementSptr second_map_ptr = make_shared<Element<ElementMap>>(second_map);

    vector_of_maps.push_back(first_map_ptr);
    vector_of_maps.push_back(second_map_ptr);
    ElementSptr vector_of_maps_ptr = make_shared<Element<ElementVector>>(vector_of_maps);

    ElementVector res_vector;
    res_vector.push_back(first_string);
    res_vector.push_back(second_string);

    EXPECT_EQ(vector_of_maps_ptr->getSubList("test"), res_vector);
}

TEST(ElementTest, getSubListDataErrorTest) {
    ElementVector vector_of_maps;

    ElementSptr first_string = make_shared<Element<std::string>>(std::string("First string"));
    map<string, shared_ptr<ListElement>> first_map {{"test", first_string}};
    ElementSptr first_map_ptr = make_shared<Element<ElementMap>>(first_map);

    ElementSptr second_string = make_shared<Element<std::string>>(std::string("Second string"));
    map<string, shared_ptr<ListElement>> second_map {{"test", second_string}};
    ElementSptr second_map_ptr = make_shared<Element<ElementMap>>(second_map);

    vector_of_maps.push_back(first_map_ptr);
    vector_of_maps.push_back(second_map_ptr);
    ElementSptr vector_of_maps_ptr = make_shared<Element<ElementVector>>(vector_of_maps);

    EXPECT_EQ(vector_of_maps_ptr->getSubList("").size(), 0);
}

TEST(ElementTest, getSubListTypeErrorTest) {
    ElementSptr first_string = make_shared<Element<std::string>>(std::string("First string"));
    map<string, shared_ptr<ListElement>> first_map {{"test", first_string}};
    ElementSptr first_map_ptr = make_shared<Element<ElementMap>>(first_map);

    EXPECT_EQ(first_map_ptr->getSubList("").size(), 0);
}

TEST(ElementTest, extendTest) {
    vector<std::shared_ptr<ListElement>> base_vector;
    vector<std::shared_ptr<ListElement>> plus_vector;
    vector<std::shared_ptr<ListElement>> res_vector;
    
    ElementSptr one   = make_shared<Element<int>>(1);
    ElementSptr two   = make_shared<Element<int>>(2);
    ElementSptr three = make_shared<Element<int>>(3);
    ElementSptr four  = make_shared<Element<int>>(4);
    
    base_vector.push_back(one);
    base_vector.push_back(two);

    plus_vector.push_back(three);
    plus_vector.push_back(four);

    res_vector.push_back(one);
    res_vector.push_back(two);
    res_vector.push_back(three);
    res_vector.push_back(four);

    ElementSptr base_vector_ptr = make_shared<Element<ElementVector>>(base_vector);
    ElementSptr plus_vector_ptr = make_shared<Element<ElementVector>>(plus_vector);
    ElementSptr res_vector_ptr  = make_shared<Element<ElementVector>>(res_vector);
    
    base_vector_ptr->extend(plus_vector_ptr);
    EXPECT_EQ(base_vector_ptr->getVector(), res_vector_ptr->getVector());
}

TEST(ElementTest, discardTest) {
    vector<std::shared_ptr<ListElement>> test_vector;
    
    ElementSptr a = make_shared<Element<int>>(32);
    ElementSptr b = make_shared<Element<int>>(2048);
    ElementSptr c = make_shared<Element<int>>(64);
    ElementSptr d = make_shared<Element<int>>(256);

    ElementSptr to_extend_ptr = make_shared<Element<ElementVector>>(vector {a, b, c, d});
    ElementSptr test_vector_ptr = make_shared<Element<ElementVector>>(test_vector);
    test_vector_ptr->extend(to_extend_ptr);
    test_vector_ptr->discard(2);

    vector<std::shared_ptr<ListElement>> res_vector {a, b};
    EXPECT_EQ(test_vector_ptr->getVector(), res_vector);
}

TEST(ElementTest, vectorCloneTest) {    
    ElementVector vec_to_clone;
    vec_to_clone.push_back(make_shared<Element<int>>(33));
    vec_to_clone.push_back(make_shared<Element<int>>(66));
    vec_to_clone.push_back(make_shared<Element<int>>(99));
    ElementSptr vec_to_clone_ptr = make_shared<Element<ElementVector>>(vec_to_clone);
    ElementSptr vec_cloned_ptr = vec_to_clone_ptr->clone();
    ElementVector vec_cloned = vec_cloned_ptr->getVector();

    EXPECT_EQ(vec_to_clone[0]->getInt(), vec_cloned[0]->getInt());
    EXPECT_EQ(vec_to_clone[1]->getInt(), vec_cloned[1]->getInt());
    EXPECT_EQ(vec_to_clone[2]->getInt(), vec_cloned[2]->getInt());
}


//================================================================
// Type: STRING
//================================================================

TEST (ElementTest, getStringTest) {
    string test_string = "This is a test";
    ElementSptr test_element_string = make_shared<Element<std::string>>(test_string);
    EXPECT_EQ(test_element_string->getString(), "This is a test");
}

TEST (ElementTest, getStringIntegralTest) {
    int test_int = 0;
    ElementSptr test_element_int = make_shared<Element<int>>(test_int);
    EXPECT_EQ(test_element_int->getString(), "0");
}

TEST (ElementTest, getStringTypeErrorTest) {
    uintptr_t test_id = 100000;
    Connection test_connection;
    test_connection.id = test_id;
    ElementSptr test_element_connection = make_shared<Element<Connection>>(test_connection);
    EXPECT_EQ(test_element_connection->getString(), "");
}

TEST(ElementTest, stringCloneTest) {
    string test_string = "this is a test";
    ElementSptr test_element_string = make_shared<Element<string>>(test_string);
    ElementSptr cloned_element_string = test_element_string->clone();
    EXPECT_EQ(test_element_string->getString(), cloned_element_string->getString());
}

TEST(ElementTest, getSizeStringTest) {
    string test_string = "this is a test";
    ElementSptr test_element_string = make_shared<Element<string>>(test_string);
    EXPECT_EQ(test_element_string->getSize(), test_string.size());
}


//================================================================
// Type: CONNECTION
//================================================================

TEST (ElementTest, getConnectionTest) {
    uintptr_t test_id = 100000;
    Connection test_connection;
    test_connection.id = test_id;
    ElementSptr test_element_connection = make_shared<Element<Connection>>(test_connection);
    EXPECT_EQ(test_element_connection->getConnection(), test_connection);
}

TEST (ElementTest, getConnectionTypeErrorTest) {
    ElementSptr test_element_string = make_shared<Element<std::string>>(std::string("this is a test"));
    uintptr_t test_id = 0;
    Connection test_connection;
    test_connection.id = test_id;
    EXPECT_EQ(test_element_string->getConnection(), test_connection);
}

TEST(ElementTest, getSizeConnectionTest) {
    uintptr_t test_id = 10;
    Connection test_connection;
    test_connection.id = test_id;
    ElementSptr test_element_connection = make_shared<Element<Connection>>(test_connection);
    EXPECT_EQ(test_element_connection->getSize(), 0);
}

TEST(ElementTest, ConnectionCloneTest) {
    uintptr_t test_id = 100000;
    Connection test_connection;
    test_connection.id = test_id;
    ElementSptr test_element_connection = make_shared<Element<Connection>>(test_connection);
    ElementSptr cloned_element_connection = test_element_connection->clone();
    EXPECT_EQ(test_element_connection->getConnection(), cloned_element_connection->getConnection());
}
