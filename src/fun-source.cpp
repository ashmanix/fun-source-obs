#include "fun-source.hpp"

FunSource::FunSource() {}
FunSource::~FunSource() {}

void FunSource::RegisterSource()
{
	source_info.id = "ashmanix_plugin_fun_source";
	source_info.type = OBS_SOURCE_TYPE_FILTER;
	source_info.output_flags = OBS_SOURCE_VIDEO;
	source_info.get_name = GetName;
	source_info.create = CreateSource;
	source_info.destroy = DestroySource;
	source_info.video_render = RenderSource;
	source_info.update = UpdateSource;
	source_info.get_properties = GetProperties;

	obs_register_source(&source_info);
}

const char *FunSource::GetName(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("PluginName");
}

void FunSource::ChangeShaderSelection(struct source_data *sourceData,
				      obs_data_t *settings)
{
	UNUSED_PARAMETER(settings);
	long long shaderIndex = sourceData->selectedShaderIndex;

	obs_enter_graphics();

	if (sourceData->effect) {
		gs_effect_destroy(sourceData->effect);
	}
	const char *effectPath =
		sourceData->shaderArray[shaderIndex]->GetShaderFilePath();
	sourceData->effect = gs_effect_create_from_file(effectPath, NULL);

	obs_leave_graphics();

	if (!sourceData->effect) {
		obs_log(LOG_ERROR, "Could not load shader file '%s'",
			effectPath);
	}

	sourceData->shaderArray[shaderIndex]->SetParameters(
		sourceData->effect);
	TogglePropertyGroupVisibility(sourceData);
}

void *FunSource::CreateSource(obs_data_t *settings, obs_source_t *source)
{
	struct source_data *sourceData =
		(struct source_data *)bzalloc(sizeof(struct source_data));

	// sourceData->shaderArray.push_back(
	// 	std::unique_ptr<BaseShader>(new SimpleGaussianFilter()));

	sourceData->context = source;
	sourceData->selectedShaderIndex = 0;

	sourceData->shaderArray[sourceData->selectedShaderIndex]->UpdateShader(
		settings);

	ChangeShaderSelection(sourceData, settings);
	SetDefaultProperties(sourceData, settings);

	obs_source_update(source, settings);

	return sourceData;
}

void FunSource::DestroySource(void *data)
{
	struct source_data *shaderData = (struct source_data *)data;
	if (shaderData->effect) {
		obs_enter_graphics();
		gs_effect_destroy(shaderData->effect);
		obs_leave_graphics();
	}
	bfree(data);
}

void FunSource::UpdateSource(void *data, obs_data_t *settings)
{
	// obs_log(LOG_INFO, "Updating Source!");
	struct source_data *sourceData = (struct source_data *)data;
	long long shaderTypeIndex = obs_data_get_int(settings, SETTING_SHADER_TYPE);
	if (sourceData && (sourceData->selectedShaderIndex != shaderTypeIndex)) {
		sourceData->selectedShaderIndex = shaderTypeIndex;
		ChangeShaderSelection(sourceData, settings);
	}
	sourceData->shaderArray[shaderTypeIndex]->UpdateShader(settings);
}

bool FunSource::ShaderSelectionChangeCallback(void *data,
					      obs_properties_t *props,
					      obs_property_t *list,
					      obs_data_t *settings)
{
	UNUSED_PARAMETER(props);
	long long shaderIndex =
		obs_data_get_int(settings, SETTING_SHADER_TYPE);

	struct source_data *shaderData = (struct source_data *)data;

	if (shaderIndex != shaderData->selectedShaderIndex) {
		shaderData->selectedShaderIndex = shaderIndex;

		ChangeShaderSelection(shaderData, settings);

		obs_log(LOG_INFO, "Shader Changed To: %s",
			obs_property_list_item_name(list, shaderIndex));
	}

	return true;
}

obs_properties_t *FunSource::GetProperties(void *data)
{
	struct source_data *sourceData = (struct source_data *)data;

	if (sourceData) {
		sourceData->mainProperties = obs_properties_create();

		obs_property_t *sourceDropdownProperty =
			obs_properties_add_list(
				sourceData->mainProperties, SETTING_SHADER_TYPE,
				obs_module_text("ShaderTypeDropdownSelectTitle"),
				OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);

		for (size_t i = 0; i < sourceData->shaderArray.size(); i++) {

			const char *name =
				sourceData->shaderArray[i]->GetShaderName();
			obs_property_list_add_int(sourceDropdownProperty, name,
						  i);
		}

		obs_property_set_modified_callback2(
			sourceDropdownProperty, ShaderSelectionChangeCallback,
			sourceData);

		std::string shaderStringName = "shader-";
		for (size_t i = 0; i < sourceData->shaderArray.size(); i++) {
			sourceData->shaderArray[i]->SetProperties(
				sourceData->mainProperties,
				shaderStringName.append(std::to_string(i)));
		}
		TogglePropertyGroupVisibility(sourceData);
		return sourceData->mainProperties;
	}

	return nullptr;
}

void FunSource::SetDefaultProperties(source_data *sourceData,
				     obs_data_t *settings)
{
	for (size_t i = 0; i < sourceData->shaderArray.size(); i++) {
		sourceData->shaderArray[i]->SetPropertyDefaults(settings);
	}
}

void FunSource::RenderSource(void *data, gs_effect_t *effect)
{
	struct source_data *sourceData = (struct source_data *)data;
	long long sourceIndex = sourceData->selectedShaderIndex;
	obs_source_t *context = sourceData->context;

	if (!obs_source_process_filter_begin(sourceData->context, GS_RGBA,
					     OBS_ALLOW_DIRECT_RENDERING))
		return;

	sourceData->shaderArray[sourceIndex]->Render(context);

	if (!sourceData->effect) {
		obs_source_process_filter_end(sourceData->context, effect, 0,
					      0);
	} else {
		obs_source_process_filter_end(sourceData->context,
					      sourceData->effect, 0, 0);
	}
}

void FunSource::TogglePropertyGroupVisibility(source_data *data)
{
	for (size_t i = 0; i < data->shaderArray.size(); i++) {
		if (data->selectedShaderIndex >= 0 &&
		    i != (size_t)data->selectedShaderIndex) {
			data->shaderArray[i]->HidePropertiesGroup();
		} else {
			data->shaderArray[i]->ShowPropertiesGroup();
		}
	}
}
