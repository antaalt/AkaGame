#pragma once

#include <Aka/Resource/Shader/Shader.h>

namespace aka {

const aka::Path ShaderPathSpriteVertex = aka::OS::cwd() + "asset/shaders/renderer/sprite.vert";
const aka::Path ShaderPathSpriteFragment = aka::OS::cwd() + "asset/shaders/renderer/sprite.frag";
const aka::Path ShaderPathTilemapVertex = aka::OS::cwd() + "asset/shaders/renderer/tilemap.vert";
const aka::Path ShaderPathTilemapFragment = aka::OS::cwd() + "asset/shaders/renderer/tilemap.frag";

const aka::ShaderKey ShaderAnimatedSpriteVertex{ ShaderPathSpriteVertex, { { "ANIMATED_SPRITE" } }, aka::ShaderType::Vertex, "main"};
const aka::ShaderKey ShaderAnimatedSpriteFragment{ ShaderPathSpriteFragment, { { "ANIMATED_SPRITE" } }, aka::ShaderType::Fragment, "main" };
const aka::ShaderKey ShaderStaticSpriteVertex{ ShaderPathSpriteVertex, {}, aka::ShaderType::Vertex, "main" };
const aka::ShaderKey ShaderStaticSpriteFragment{ ShaderPathSpriteFragment, {}, aka::ShaderType::Fragment, "main" };
const aka::ShaderKey ShaderTilemapVertex{ ShaderPathTilemapVertex, {}, aka::ShaderType::Vertex, "main" };
const aka::ShaderKey ShaderTilemapFragment{ ShaderPathTilemapFragment, {}, aka::ShaderType::Fragment, "main" };

const aka::ProgramKey ProgramAnimatedSprite{ {{ShaderAnimatedSpriteVertex, ShaderAnimatedSpriteFragment}} };
const aka::ProgramKey ProgramStaticSprite{ {{ShaderStaticSpriteVertex, ShaderStaticSpriteFragment}} };
const aka::ProgramKey ProgramTilemap{ {{ShaderTilemapVertex, ShaderTilemapFragment}} };


};