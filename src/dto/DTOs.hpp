#ifndef DTOs_hpp
#define DTOs_hpp

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include <AppComponent.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */
class DefaultDTO : public oatpp::DTO {

	DTO_INIT(DefaultDTO, DTO)

	DTO_FIELD(Int32, statusCode);
	DTO_FIELD(String, message);

};

static std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> error500(std::string answer = "something bad happend :c")
{
	DefaultDTO::Wrapper def = DefaultDTO::createShared();
	def->statusCode = 500;
	def->message = answer.c_str();
	
	OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, m_defaultObjectMapper);
	return oatpp::web::protocol::http::outgoing::ResponseFactory::createResponse(oatpp::web::protocol::http::Status::CODE_500, def, m_defaultObjectMapper);
}

class EmptyDTO : public oatpp::DTO {

	DTO_INIT(EmptyDTO, DTO)

	DTO_FIELD(Int32, statusCode);
	DTO_FIELD(String, result);

};

#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */
