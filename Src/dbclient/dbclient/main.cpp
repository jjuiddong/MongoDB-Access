
#include <iostream>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

int main(int, char**) {
	mongocxx::instance inst{};
	mongocxx::client conn{ mongocxx::uri{} };

	bsoncxx::builder::stream::document document{};

	auto collection = conn["blog"]["users"];
	
	//document << "hello" << "world";
	document << "username" << "test id";
	document << "hashedPassword" << "password~~";

	collection.insert_one(document.view());

	auto cursor = collection.find({});

	for (auto&& doc : cursor) {
		std::cout << bsoncxx::to_json(doc) << std::endl;
	}
}
