#include <common4esl/config/context/EntryImpl.h>
#include <common4esl/config/FilePosition.h>

namespace common4esl {
inline namespace v1_6 {
namespace config {
namespace context {

EntryImpl::EntryImpl(const std::string& fileName, const tinyxml2::XMLElement& element)
: Entry(fileName, element)
{
	if(element.Name() == nullptr) {
		throw FilePosition::add(*this, "Element name is empty");
	}

	std::string elementName(element.Name());

	if(elementName == "basic-client") {
		basicClient = std::unique_ptr<BasicClient>(new BasicClient(getFileName(), element));
	}
	else if(elementName == "basic-server") {
		basicServer = std::unique_ptr<BasicServer>(new BasicServer(getFileName(), element));
	}
	else if(elementName == "database") {
		database = std::unique_ptr<Database>(new Database(getFileName(), element));
	}
	else if(elementName == "http-client") {
		httpClient = std::unique_ptr<HttpClient>(new HttpClient(getFileName(), element));
	}
	else if(elementName == "http-server") {
		httpServer = std::unique_ptr<HttpServer>(new HttpServer(getFileName(), element));
	}
	else if(elementName == "object") {
		object = std::unique_ptr<Object>(new Object(getFileName(), element));
	}
	else if(elementName == "procedure") {
		procedure = std::unique_ptr<Procedure>(new Procedure(getFileName(), element));
	}
	else {
		throw FilePosition::add(*this, "Unknown element name \"" + elementName + "\".");
	}
}

void EntryImpl::save(std::ostream& oStream, std::size_t spaces) const {
	if(basicClient) {
		basicClient->save(oStream, spaces);
	}
	if(basicServer) {
		basicServer->save(oStream, spaces);
	}
	if(database) {
		database->save(oStream, spaces);
	}
	if(httpClient) {
		httpClient->save(oStream, spaces);
	}
	if(httpServer) {
		httpServer->save(oStream, spaces);
	}
	if(object) {
		object->save(oStream, spaces);
	}
	if(procedure) {
		procedure->save(oStream, spaces);
	}
}

void EntryImpl::install(object::ProcessingContext& context) const {
	if(basicClient) {
		basicClient->install(context);
	}
	if(basicServer) {
		basicServer->install(context);
	}
	if(database) {
		database->install(context);
	}
	if(httpClient) {
		httpClient->install(context);
	}
	if(httpServer) {
		httpServer->install(context);
	}
	if(object) {
		object->install(context);
	}
	if(procedure) {
		procedure->install(context);
	}
}

} /* namespace context */
} /* namespace config */
} /* inline namespace v1_6 */
} /* namespace common4esl */
