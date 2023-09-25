#ifndef FUNSOURCE_H
#define FUNSOURCE_H

#include <obs-module.h>
#include <plugin-support.h>
#include <string>
#include <vector>
#include <memory>
#include "src/shaders/base-shader.hpp"

#define SETTING_SHADER_TYPE "shader_type"

class FunSource {
private:
	struct source_data {
		obs_source_t *context;
		gs_effect_t *effect;

		char *selectedFileName;
		long long selectedShaderIndex;
		std::vector<std::unique_ptr<BaseShader>> shaderArray;

		obs_properties_t *mainProperties;
		obs_properties_t *sourceProperties;
		obs_property_t *sourcePropertiesGroup;
	};

	obs_source_info source_info = {};

	static const char *GetName(void *unused);
	static void *CreateSource(obs_data_t *settings, obs_source_t *source);
	static void DestroySource(void *data);
	static void UpdateSource(void *data, obs_data_t *settings);
	static obs_properties_t *GetProperties(void *data);
	static void SetDefaultProperties(source_data *sourceData,
					 obs_data_t *settings);
	static void RenderSource(void *data, gs_effect_t *effect);
	static void ChangeShaderSelection(struct source_data *filter,
					  obs_data_t *settings);
	static bool ShaderSelectionChangeCallback(void *data,
						  obs_properties_t *props,
						  obs_property_t *list,
						  obs_data_t *settings);
	static void TogglePropertyGroupVisibility(source_data *data);

public:
	FunSource();
	~FunSource();
	void RegisterSource();
};

#endif // FUNSOURCE_H
