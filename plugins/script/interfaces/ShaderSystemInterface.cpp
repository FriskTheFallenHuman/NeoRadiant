#include "ShaderSystemInterface.h"
#include "../LuaHelper.h"

namespace script
{

constexpr const char* META_MATERIAL		= "NeoRadiant.Material";
constexpr const char* META_MAT_STAGE	= "NeoRadiant.MaterialStage";
constexpr const char* META_MAT_STAGE_ED = "NeoRadiant.EditableMaterialStage";

void ShaderSystemInterface::registerInterface( lua_State* L )
{
	// IShaderLayer
	static luaL_Reg stageMethods[] =
		{ { "getType",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getType() );
				return 1;
			} },
		{ "getMapType",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getMapType() );
				return 1;
			} },
		{ "getMapExpressionString",
			[](lua_State* L)->int {
				auto expr		   = lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getMapExpression();
				lua_pushstdstring( L, expr ? expr->getExpressionString() : std::string() );
				return 1;
			} },
		{ "getStageFlags",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getStageFlags() );
				return 1;
			} },
		{ "getClampType",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getClampType() );
				return 1;
			} },
		{ "getTexGenType",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getTexGenType() );
				return 1;
			} },
		{ "getTexGenExpressionString",
			[](lua_State* L)->int {
				std::size_t idx			   = ( std::size_t )luaL_checkinteger( L, 2 );
				auto expr		   = lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getTexGenExpression( idx );
				lua_pushstdstring( L, expr ? expr->getExpressionString() : std::string() );
				return 1;
			} },
		{ "getColourExpressionString",
			[](lua_State* L)->int {
				auto comp = static_cast<IShaderLayer::ColourComponentSelector>( luaL_checkinteger( L, 2 ) );
				const auto& expr		   = lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getColourExpression( comp );
				lua_pushstdstring( L, expr ? expr->getExpressionString() : std::string() );
				return 1;
			} },
		{ "getVertexColourMode",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getVertexColourMode() );
				return 1;
			} },
		{ "getAlphaTestExpressionString",
			[](lua_State* L)->int {
				const auto& expr = lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getAlphaTestExpression();
				lua_pushstdstring( L, expr ? expr->getExpressionString() : std::string() );
				return 1;
			} },
		{ "getConditionExpressionString",
			[](lua_State* L)->int {
				const auto& expr = lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getConditionExpression();
				lua_pushstdstring( L, expr ? expr->getExpressionString() : std::string() );
				return 1;
			} },
		{ "getVertexProgram",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getVertexProgram() );
				return 1;
			} },
		{ "getFragmentProgram",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getFragmentProgram() );
				return 1;
			} },
		{ "getNumVertexParms",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getNumVertexParms() );
				return 1;
			} },
		{ "getNumFragmentMaps",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getNumFragmentMaps() );
				return 1;
			} },
		{ "getPrivatePolygonOffset",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getPrivatePolygonOffset() );
				return 1;
			} },
		{ "getRenderMapSize",
			[](lua_State* L)->int {
				const auto& sz			   = lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getRenderMapSize();
				lua_newtable( L );
				lua_pushnumber( L, sz.x() );
				lua_setfield( L, -2, "x" );
				lua_pushnumber( L, sz.y() );
				lua_setfield( L, -2, "y" );
				return 1;
			} },
		{ "getBlendFuncStrings",
			[](lua_State* L)->int {
				const auto& bf = lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getBlendFuncStrings();
				lua_newtable( L );
				lua_pushstdstring( L, bf.first );
				lua_rawseti( L, -2, 1 );
				lua_pushstdstring( L, bf.second );
				lua_rawseti( L, -2, 2 );
				return 1;
			} },
		{ "getTransformations",
			[](lua_State* L)->int {
				const auto& transforms = lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getTransformations();
				lua_newtable( L );
				for( int i = 0; i < ( int )transforms.size(); ++i ) {
					const auto& t = transforms[i];
					lua_newtable( L );
					lua_pushinteger( L, ( lua_Integer )t.type );
					lua_setfield( L, -2, "type" );
					lua_pushstdstring( L, t.expression1 ? t.expression1->getExpressionString() : "" );
					lua_setfield( L, -2, "expression1" );
					lua_pushstdstring( L, t.expression2 ? t.expression2->getExpressionString() : "" );
					lua_setfield( L, -2, "expression2" );
					lua_rawseti( L, -2, i + 1 );
				}
				return 1;
			} },
		{ "getVertexParm",
			[](lua_State* L)->int {
				int idx = ( int )luaL_checkinteger( L, 2 );
				const auto& vp = lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getVertexParm( idx );
				lua_newtable( L );
				lua_pushinteger( L, vp.index );
				lua_setfield( L, -2, "index" );
				lua_newtable( L );
				for( int i = 0; i < 4 && vp.expressions[i]; ++i ) {
					lua_pushstdstring( L, vp.expressions[i]->getExpressionString() );
					lua_rawseti( L, -2, i + 1 );
				}
				lua_setfield( L, -2, "expressions" );
				return 1;
			} },
		{ "getFragmentMap",
			[](lua_State* L)->int {
				int idx = ( int )luaL_checkinteger( L, 2 );
				const auto& fm = lua_checkobject<IShaderLayer>( L, 1, META_MAT_STAGE )->getFragmentMap( idx );
				lua_newtable( L );
				lua_pushinteger( L, fm.index );
				lua_setfield( L, -2, "index" );
				lua_newtable( L );
				for( int i = 0; i < ( int )fm.options.size(); ++i ) {
					lua_pushstdstring( L, fm.options[i] );
					lua_rawseti( L, -2, i + 1 );
				}
				lua_setfield( L, -2, "options" );
				lua_pushstdstring( L, fm.map ? fm.map->getExpressionString() : "" );
				lua_setfield( L, -2, "mapExpression" );
				return 1;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_MAT_STAGE, stageMethods );

	// IEditableShaderLayer
	static luaL_Reg editStageMethods[] = {
		{ "setStageFlag",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setStageFlag( static_cast<IShaderLayer::Flags>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "clearStageFlag",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->clearStageFlag( static_cast<IShaderLayer::Flags>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "setMapType",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setMapType( static_cast<IShaderLayer::MapType>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "setMapExpressionFromString",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setMapExpressionFromString( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "setBlendFuncStrings",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TTABLE );
				lua_rawgeti( L, 2, 1 );
				std::string s1 = lua_tostring( L, -1 ) ? lua_tostring( L, -1 ) : "";
				lua_pop( L, 1 );
				lua_rawgeti( L, 2, 2 );
				std::string s2 = lua_tostring( L, -1 ) ? lua_tostring( L, -1 ) : "";
				lua_pop( L, 1 );
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setBlendFuncStrings( { s1, s2 } );
				return 0;
			} },
		{ "setAlphaTestExpressionFromString",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setAlphaTestExpressionFromString( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "addTransformation",
			[](lua_State* L)->int {
				auto type = static_cast<IShaderLayer::TransformType>( ( int )luaL_checkinteger( L, 2 ) );
				lua_pushinteger(
					L, ( lua_Integer )lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->addTransformation( type, lua_checkstdstring( L, 3 ), lua_checkstdstring( L, 4 ) ) );
				return 1;
			} },
		{ "updateTransformation",
			[](lua_State* L)->int {
				std::size_t idx = ( std::size_t )luaL_checkinteger( L, 2 );
				auto type = static_cast<IShaderLayer::TransformType>( ( int )luaL_checkinteger( L, 3 ) );
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->updateTransformation( idx, type, lua_checkstdstring( L, 4 ), lua_checkstdstring( L, 5 ) );
				return 0;
			} },
		{ "removeTransformation",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->removeTransformation( ( std::size_t )luaL_checkinteger( L, 2 ) );
				return 0;
			} },
		{ "setColourExpressionFromString",
			[](lua_State* L)->int {
				auto comp = static_cast<IShaderLayer::ColourComponentSelector>( ( int )luaL_checkinteger( L, 2 ) );
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setColourExpressionFromString( comp, lua_checkstdstring( L, 3 ) );
				return 0;
			} },
		{ "setConditionExpressionFromString",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setConditionExpressionFromString( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "setTexGenType",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setTexGenType( static_cast<IShaderLayer::TexGenType>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "setTexGenExpressionFromString",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setTexGenExpressionFromString( ( std::size_t )luaL_checkinteger( L, 2 ), lua_checkstdstring( L, 3 ) );
				return 0;
			} },
		{ "setVertexColourMode",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setVertexColourMode( static_cast<IShaderLayer::VertexColourMode>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "setClampType",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setClampType( static_cast<ClampType>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "setPrivatePolygonOffset",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setPrivatePolygonOffset( ( double )luaL_checknumber( L, 2 ) );
				return 0;
			} },
		{ "setRenderMapSize",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setRenderMapSize( Vector2( ( float )luaL_checknumber( L, 2 ), ( float )luaL_checknumber( L, 3 ) ) );
				return 0;
			} },
		{ "setSoundMapWaveForm",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setSoundMapWaveForm( lua_toboolean( L, 2 ) != 0 );
				return 0;
			} },
		{ "setVideoMapProperties",
			[](lua_State* L)->int {
				lua_checkobject<IEditableShaderLayer>( L, 1, META_MAT_STAGE_ED )->setVideoMapProperties( lua_checkstdstring( L, 2 ), lua_toboolean( L, 3 ) != 0 );
				return 0;
			} },
		{ nullptr, nullptr }
	};
	lua_registerclass( L, META_MAT_STAGE_ED, editStageMethods );

	// Material
	static luaL_Reg matMethods[] =
		{ { "isNull",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->IsDefault() );
				return 1;
			} },
		{ "getName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->getName() );
				return 1;
			} },
		{ "getShaderFileName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->getShaderFileName() );
				return 1;
			} },
		{ "getDescription",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->getDescription() );
				return 1;
			} },
		{ "isModified",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->isModified() );
				return 1;
			} },
		{ "getDefinition",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->getDefinition() );
				return 1;
			} },
		{ "isVisible",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->isVisible() );
				return 1;
			} },
		{ "isAmbientLight",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->isAmbientLight() );
				return 1;
			} },
		{ "isBlendLight",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->isBlendLight() );
				return 1;
			} },
		{ "isFogLight",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->isFogLight() );
				return 1;
			} },
		{ "isCubicLight",
			[](lua_State* L)->int {
				lua_pushboolean( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->isCubicLight() );
				return 1;
			} },
		{ "getSortRequest",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->getSortRequest() );
				return 1;
			} },
		{ "getPolygonOffset",
			[](lua_State* L)->int {
				lua_pushnumber( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->getPolygonOffset() );
				return 1;
			} },
		{ "getClampType",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<Material>( L, 1, META_MATERIAL )->getClampType() );
				return 1;
			} },
		{ "getCullType",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<Material>( L, 1, META_MATERIAL )->getCullType() );
				return 1;
			} },
		{ "getMaterialFlags",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<Material>( L, 1, META_MATERIAL )->getMaterialFlags() );
				return 1;
			} },
		{ "getSurfaceFlags",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<Material>( L, 1, META_MATERIAL )->getSurfaceFlags() );
				return 1;
			} },
		{ "getSurfaceType",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<Material>( L, 1, META_MATERIAL )->getSurfaceType() );
				return 1;
			} },
		{ "getDeformType",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<Material>( L, 1, META_MATERIAL )->getDeformType() );
				return 1;
			} },
		{ "getDeformExpressionString",
			[](lua_State* L)->int {
				int idx = ( int )luaL_checkinteger( L, 2 );
				auto expr = lua_checkobject<Material>( L, 1, META_MATERIAL )->getDeformExpression( idx );
				lua_pushstdstring( L, expr ? expr->getExpressionString() : std::string() );
				return 1;
			} },
		{ "getDeformDeclName",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->getDeformDeclName() );
				return 1;
			} },
		{ "getSpectrum",
			[](lua_State* L)->int {
				lua_pushinteger( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->getSpectrum() );
				return 1;
			} },
		{ "getDecalInfo",
			[](lua_State* L)->int {
				const auto& di = lua_checkobject<Material>( L, 1, META_MATERIAL )->getDecalInfo();
				lua_newtable( L );
				lua_pushinteger( L, di.stayMilliSeconds );
				lua_setfield( L, -2, "stayMilliSeconds" );
				lua_pushinteger( L, di.fadeMilliSeconds );
				lua_setfield( L, -2, "fadeMilliSeconds" );
				lua_newtable( L );
				lua_pushnumber( L, di.startColour.x() );
				lua_setfield( L, -2, "r" );
				lua_pushnumber( L, di.startColour.y() );
				lua_setfield( L, -2, "g" );
				lua_pushnumber( L, di.startColour.z() );
				lua_setfield( L, -2, "b" );
				lua_pushnumber( L, di.startColour.w() );
				lua_setfield( L, -2, "a" );
				lua_setfield( L, -2, "startColour" );
				lua_newtable( L );
				lua_pushnumber( L, di.endColour.x() );
				lua_setfield( L, -2, "r" );
				lua_pushnumber( L, di.endColour.y() );
				lua_setfield( L, -2, "g" );
				lua_pushnumber( L, di.endColour.z() );
				lua_setfield( L, -2, "b" );
				lua_pushnumber( L, di.endColour.w() );
				lua_setfield( L, -2, "a" );
				lua_setfield( L, -2, "endColour" );
				return 1;
			} },
		{ "getCoverage",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<Material>( L, 1, META_MATERIAL )->getCoverage() );
				return 1;
			} },
		{ "getLightFalloffCubeMapType",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<Material>( L, 1, META_MATERIAL )->getLightFalloffCubeMapType() );
				return 1;
			} },
		{ "getRenderBumpArguments",
			[](lua_State* L)->int {
				lua_pushstdstring( L, lua_checkobject<Material>( L, 1, META_MATERIAL )->getRenderBumpArguments() );
				return 1;
			} },
		{ "getNumStages",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<Material>( L, 1, META_MATERIAL )->getNumLayers() );
				return 1;
			} },
		{ "getStage",
			[](lua_State* L)->int {
				int idx = ( int )luaL_checkinteger( L, 2 );
				auto layer = lua_checkobject<Material>( L, 1, META_MATERIAL )->getLayer( idx );
				if( !layer ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, layer.get(), META_MAT_STAGE );
				return 1;
			} },
		{ "getAllStages",
			[](lua_State* L)->int {
				auto* m = lua_checkobject<Material>( L, 1, META_MATERIAL );
				lua_newtable( L );
				int n = ( int )m->getNumLayers();
				for( int i = 0; i < n; ++i ) {
					auto layer = m->getLayer( i );
					if( layer ) {
						lua_pushobject( L, layer.get(), META_MAT_STAGE );
						lua_rawseti( L, -2, i + 1 );
					}
				}
				return 1;
			} },
		{ "getEditableStage",
			[](lua_State* L)->int {
				int idx = ( int )luaL_checkinteger( L, 2 );
				auto layer = lua_checkobject<Material>( L, 1, META_MATERIAL )->getEditableLayer( idx );
				if( !layer ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, layer.get(), META_MAT_STAGE_ED );
				return 1;
			} },
		{ "setShaderFileName",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setShaderFileName( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "setDescription",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setDescription( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "setEditorImageExpressionFromString",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setEditorImageExpressionFromString( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "setSortRequest",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setSortRequest( ( float )luaL_checknumber( L, 2 ) );
				return 0;
			} },
		{ "setPolygonOffset",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setPolygonOffset( ( float )luaL_checknumber( L, 2 ) );
				return 0;
			} },
		{ "setClampType",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setClampType( static_cast<ClampType>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "setCullType",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setCullType( static_cast<Material::CullType>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "setMaterialFlag",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setMaterialFlag( static_cast<Material::Flags>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "clearMaterialFlag",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->clearMaterialFlag( static_cast<Material::Flags>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "setSurfaceFlag",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setSurfaceFlag( static_cast<Material::SurfaceFlags>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "clearSurfaceFlag",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->clearSurfaceFlag( static_cast<Material::SurfaceFlags>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "setSurfaceType",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setSurfaceType( static_cast<Material::SurfaceType>( ( int )luaL_checkinteger( L, 2 ) ) );
				return 0;
			} },
		{ "setSpectrum",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setSpectrum( ( int )luaL_checkinteger( L, 2 ) );
				return 0;
			} },
		{ "setIsAmbientLight",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setIsAmbientLight( lua_toboolean( L, 2 ) != 0 );
				return 0;
			} },
		{ "setIsBlendLight",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setIsBlendLight( lua_toboolean( L, 2 ) != 0 );
				return 0;
			} },
		{ "setIsFogLight",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setIsFogLight( lua_toboolean( L, 2 ) != 0 );
				return 0;
			} },
		{ "setIsCubicLight",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->setIsCubicLight( lua_toboolean( L, 2 ) != 0 );
				return 0;
			} },
		{ "addStage",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<Material>( L, 1, META_MATERIAL )->addLayer( static_cast<IShaderLayer::Type>( ( int )luaL_checkinteger( L, 2 ) ) ) );
				return 1;
			} },
		{ "removeStage",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->removeLayer( ( int )luaL_checkinteger( L, 2 ) );
				return 0;
			} },
		{ "duplicateStage",
			[](lua_State* L)->int {
				lua_pushinteger( L, ( lua_Integer )lua_checkobject<Material>( L, 1, META_MATERIAL )->duplicateLayer( ( int )luaL_checkinteger( L, 2 ) ) );
				return 1;
			} },
		{ "swapStagePosition",
			[](lua_State* L)->int {
				lua_checkobject<Material>( L, 1, META_MATERIAL )->swapLayerPosition( ( int )luaL_checkinteger( L, 2 ), ( int )luaL_checkinteger( L, 3 ) );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, META_MATERIAL, matMethods );

	// GlobalMaterialManager
	static luaL_Reg sys[] =
		{ { "getMaterial",
			[](lua_State* L)->int {
				auto mat = GlobalMaterialManager().getMaterial( lua_checkstdstring( L, 2 ) );
				if( !mat ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, mat.get(), META_MATERIAL );
				return 1;
			} },
		{ "getMaterialForName",
			[](lua_State* L)->int {
				auto mat	  = GlobalMaterialManager().getMaterial( lua_checkstdstring( L, 2 ) );
				if( !mat ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, mat.get(), META_MATERIAL );
				return 1;
			} },
		{ "materialExists",
			[](lua_State* L)->int {
				lua_pushboolean( L, GlobalMaterialManager().materialExists( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ "materialCanBeModified",
			[](lua_State* L)->int {
				lua_pushboolean( L, GlobalMaterialManager().materialCanBeModified( lua_checkstdstring( L, 2 ) ) );
				return 1;
			} },
		{ "createEmptyMaterial",
			[](lua_State* L)->int {
				auto mat	  = GlobalMaterialManager().createEmptyMaterial( lua_checkstdstring( L, 2 ) );
				if( !mat ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, mat.get(), META_MATERIAL );
				return 1;
			} },
		{ "copyMaterial",
			[](lua_State* L)->int {
				auto mat	  = GlobalMaterialManager().copyMaterial( lua_checkstdstring( L, 2 ), lua_checkstdstring( L, 3 ) );
				if( !mat ) {
					lua_pushnil( L );
					return 1;
				}
				lua_pushobject( L, mat.get(), META_MATERIAL );
				return 1;
			} },
		{ "renameMaterial",
			[](lua_State* L)->int {
				lua_pushboolean( L, GlobalMaterialManager().renameMaterial( lua_checkstdstring( L, 2 ), lua_checkstdstring( L, 3 ) ) );
				return 1;
			} },
		{ "removeMaterial",
			[](lua_State* L)->int {
				GlobalMaterialManager().removeMaterial( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "saveMaterial",
			[](lua_State* L)->int {
				GlobalMaterialManager().saveMaterial( lua_checkstdstring( L, 2 ) );
				return 0;
			} },
		{ "foreachMaterial",
			[](lua_State* L)->int {
				luaL_checktype( L, 2, LUA_TFUNCTION );
				lua_pushvalue( L, 2 );
				int ref	  = luaL_ref( L, LUA_REGISTRYINDEX );
				GlobalMaterialManager().foreachMaterial( [&]( const MaterialPtr& m ) {
					lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
					lua_pushobject( L, m.get(), META_MATERIAL );
					if( lua_pcall( L, 1, 0, 0 ) != LUA_OK )
						lua_pop( L, 1 );
				} );
				luaL_unref( L, LUA_REGISTRYINDEX, ref );
				return 0;
			} },
		{ nullptr, nullptr } };
	lua_registerclass( L, "NeoRadiant.MaterialManager", sys );
	lua_setglobal_object( L, "GlobalMaterialManager", this, "NeoRadiant.MaterialManager" );

	// Material.* enum tables
	lua_newtable( L );

	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )Material::SURFTYPE_DEFAULT );
	lua_setfield( L, -2, "DEFAULT" );
	lua_pushinteger( L, ( lua_Integer )Material::SURFTYPE_METAL );
	lua_setfield( L, -2, "METAL" );
	lua_pushinteger( L, ( lua_Integer )Material::SURFTYPE_STONE );
	lua_setfield( L, -2, "STONE" );
	lua_pushinteger( L, ( lua_Integer )Material::SURFTYPE_FLESH );
	lua_setfield( L, -2, "FLESH" );
	lua_pushinteger( L, ( lua_Integer )Material::SURFTYPE_WOOD );
	lua_setfield( L, -2, "WOOD" );
	lua_pushinteger( L, ( lua_Integer )Material::SURFTYPE_CARDBOARD );
	lua_setfield( L, -2, "CARDBOARD" );
	lua_pushinteger( L, ( lua_Integer )Material::SURFTYPE_LIQUID );
	lua_setfield( L, -2, "LIQUID" );
	lua_pushinteger( L, ( lua_Integer )Material::SURFTYPE_GLASS );
	lua_setfield( L, -2, "GLASS" );
	lua_pushinteger( L, ( lua_Integer )Material::SURFTYPE_PLASTIC );
	lua_setfield( L, -2, "PLASTIC" );
	lua_pushinteger( L, ( lua_Integer )Material::SURFTYPE_RICOCHET );
	lua_setfield( L, -2, "RICOCHET" );
	lua_setfield( L, -2, "SurfaceType" );

	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )Material::FLAG_NOSHADOWS );
	lua_setfield( L, -2, "NOSHADOWS" );
	lua_pushinteger( L, ( lua_Integer )Material::FLAG_NOSELFSHADOW );
	lua_setfield( L, -2, "NOSELFSHADOW" );
	lua_pushinteger( L, ( lua_Integer )Material::FLAG_FORCESHADOWS );
	lua_setfield( L, -2, "FORCESHADOWS" );
	lua_pushinteger( L, ( lua_Integer )Material::FLAG_NOOVERLAYS );
	lua_setfield( L, -2, "NOOVERLAYS" );
	lua_pushinteger( L, ( lua_Integer )Material::FLAG_FORCEOVERLAYS );
	lua_setfield( L, -2, "FORCEOVERLAYS" );
	lua_pushinteger( L, ( lua_Integer )Material::FLAG_TRANSLUCENT );
	lua_setfield( L, -2, "TRANSLUCENT" );
	lua_pushinteger( L, ( lua_Integer )Material::FLAG_FORCEOPAQUE );
	lua_setfield( L, -2, "FORCEOPAQUE" );
	lua_pushinteger( L, ( lua_Integer )Material::FLAG_NOFOG );
	lua_setfield( L, -2, "NOFOG" );
	lua_pushinteger( L, ( lua_Integer )Material::FLAG_MIRROR );
	lua_setfield( L, -2, "MIRROR" );
	lua_setfield( L, -2, "Flag" );

	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )Material::SURF_SOLID );
	lua_setfield( L, -2, "SOLID" );
	lua_pushinteger( L, ( lua_Integer )Material::SURF_WATER );
	lua_setfield( L, -2, "WATER" );
	lua_pushinteger( L, ( lua_Integer )Material::SURF_PLAYERCLIP );
	lua_setfield( L, -2, "PLAYERCLIP" );
	lua_pushinteger( L, ( lua_Integer )Material::SURF_MONSTERCLIP );
	lua_setfield( L, -2, "MONSTERCLIP" );
	lua_pushinteger( L, ( lua_Integer )Material::SURF_NONSOLID );
	lua_setfield( L, -2, "NONSOLID" );
	lua_pushinteger( L, ( lua_Integer )Material::SURF_AREAPORTAL );
	lua_setfield( L, -2, "AREAPORTAL" );
	lua_pushinteger( L, ( lua_Integer )Material::SURF_SLICK );
	lua_setfield( L, -2, "SLICK" );
	lua_pushinteger( L, ( lua_Integer )Material::SURF_COLLISION );
	lua_setfield( L, -2, "COLLISION" );
	lua_pushinteger( L, ( lua_Integer )Material::SURF_LADDER );
	lua_setfield( L, -2, "LADDER" );
	lua_setfield( L, -2, "SurfaceFlag" );

	// ClampType
	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )CLAMP_REPEAT );
	lua_setfield( L, -2, "REPEAT" );
	lua_pushinteger( L, ( lua_Integer )CLAMP_NOREPEAT );
	lua_setfield( L, -2, "NOREPEAT" );
	lua_pushinteger( L, ( lua_Integer )CLAMP_ZEROCLAMP );
	lua_setfield( L, -2, "ZEROCLAMP" );
	lua_pushinteger( L, ( lua_Integer )CLAMP_ALPHAZEROCLAMP );
	lua_setfield( L, -2, "ALPHAZEROCLAMP" );
	lua_setfield( L, -2, "ClampType" );

	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )Material::CULL_BACK );
	lua_setfield( L, -2, "BACK" );
	lua_pushinteger( L, ( lua_Integer )Material::CULL_FRONT );
	lua_setfield( L, -2, "FRONT" );
	lua_pushinteger( L, ( lua_Integer )Material::CULL_NONE );
	lua_setfield( L, -2, "NONE" );
	lua_setfield( L, -2, "CullType" );

	lua_newtable( L );
	lua_pushnumber( L, ( lua_Number )Material::SORT_NEAREST );
	lua_setfield( L, -2, "NEAREST" );
	lua_pushnumber( L, ( lua_Number )Material::SORT_MEDIUM );
	lua_setfield( L, -2, "MEDIUM" );
	lua_pushnumber( L, ( lua_Number )Material::SORT_CLOSE );
	lua_setfield( L, -2, "CLOSE" );
	lua_pushnumber( L, ( lua_Number )Material::SORT_FAR );
	lua_setfield( L, -2, "FAR" );
	lua_pushnumber( L, ( lua_Number )Material::SORT_DECAL );
	lua_setfield( L, -2, "DECAL" );
	lua_pushnumber( L, ( lua_Number )Material::SORT_PORTAL_SKY );
	lua_setfield( L, -2, "PORTAL_SKY" );
	lua_pushnumber( L, ( lua_Number )Material::SORT_ALMOST_NEAREST );
	lua_setfield( L, -2, "ALMOST_NEAREST" );
	lua_setfield( L, -2, "SortRequest" );

	lua_setglobal( L, "Material" );

	lua_newtable( L );

	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::BUMP );
	lua_setfield( L, -2, "BUMP" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::DIFFUSE );
	lua_setfield( L, -2, "DIFFUSE" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::SPECULAR );
	lua_setfield( L, -2, "SPECULAR" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::BLEND );
	lua_setfield( L, -2, "BLEND" );
	lua_setfield( L, -2, "Type" );

	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::MapType::Map );
	lua_setfield( L, -2, "MAP" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::MapType::CubeMap );
	lua_setfield( L, -2, "CUBEMAP" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::MapType::CameraCubeMap );
	lua_setfield( L, -2, "CAMERACUBEMAP" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::MapType::VideoMap );
	lua_setfield( L, -2, "VIDEOMAP" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::MapType::SoundMap );
	lua_setfield( L, -2, "SOUNDMAP" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::MapType::MirrorRenderMap );
	lua_setfield( L, -2, "MIRRORRENDERMAP" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::MapType::RemoteRenderMap );
	lua_setfield( L, -2, "REMOTERENDERMAP" );
	lua_setfield( L, -2, "MapType" );

	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::TEXGEN_NORMAL );
	lua_setfield( L, -2, "NORMAL" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::TEXGEN_REFLECT );
	lua_setfield( L, -2, "REFLECT" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::TEXGEN_SCREEN );
	lua_setfield( L, -2, "SCREEN" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::TEXGEN_SKYBOX );
	lua_setfield( L, -2, "SKYBOX" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::TEXGEN_WOBBLESKY );
	lua_setfield( L, -2, "WOBBLESKY" );
	lua_setfield( L, -2, "TexGenType" );

	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::COMP_RED );
	lua_setfield( L, -2, "RED" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::COMP_GREEN );
	lua_setfield( L, -2, "GREEN" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::COMP_BLUE );
	lua_setfield( L, -2, "BLUE" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::COMP_ALPHA );
	lua_setfield( L, -2, "ALPHA" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::COMP_RGB );
	lua_setfield( L, -2, "RGB" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::COMP_RGBA );
	lua_setfield( L, -2, "RGBA" );
	lua_setfield( L, -2, "ColourComponent" );

	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::VERTEX_COLOUR_NONE );
	lua_setfield( L, -2, "NONE" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::VERTEX_COLOUR_MULTIPLY );
	lua_setfield( L, -2, "MULTIPLY" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::VERTEX_COLOUR_INVERSE_MULTIPLY );
	lua_setfield( L, -2, "INVERSE_MULTIPLY" );
	lua_setfield( L, -2, "VertexColourMode" );

	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::TransformType::Translate );
	lua_setfield( L, -2, "TRANSLATE" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::TransformType::Scale );
	lua_setfield( L, -2, "SCALE" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::TransformType::Rotate );
	lua_setfield( L, -2, "ROTATE" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::TransformType::CenterScale );
	lua_setfield( L, -2, "CENTERSCALE" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::TransformType::Shear );
	lua_setfield( L, -2, "SHEAR" );
	lua_setfield( L, -2, "TransformType" );

	// IShaderLayer::Flags
	lua_newtable( L );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_IGNORE_ALPHATEST );
	lua_setfield( L, -2, "IGNORE_ALPHATEST" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_FILTER_NEAREST );
	lua_setfield( L, -2, "FILTER_NEAREST" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_FILTER_LINEAR );
	lua_setfield( L, -2, "FILTER_LINEAR" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_HIGHQUALITY );
	lua_setfield( L, -2, "HIGHQUALITY" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_FORCE_HIGHQUALITY );
	lua_setfield( L, -2, "FORCE_HIGHQUALITY" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_NO_PICMIP );
	lua_setfield( L, -2, "NO_PICMIP" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_MASK_RED );
	lua_setfield( L, -2, "MASK_RED" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_MASK_GREEN );
	lua_setfield( L, -2, "MASK_GREEN" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_MASK_BLUE );
	lua_setfield( L, -2, "MASK_BLUE" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_MASK_ALPHA );
	lua_setfield( L, -2, "MASK_ALPHA" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_MASK_DEPTH );
	lua_setfield( L, -2, "MASK_DEPTH" );
	lua_pushinteger( L, ( lua_Integer )IShaderLayer::FLAG_IGNORE_DEPTH );
	lua_setfield( L, -2, "IGNORE_DEPTH" );
	lua_setfield( L, -2, "Flag" );

	lua_setglobal( L, "MaterialStage" );
}

} // namespace script
