#ifndef BASESHADER_H
#define BASESHADER_H

#include <obs.h>
#include <string>
#include <memory>

class BaseShader {

private:
	char *shaderFileName;
	obs_property_t *shaderPropertiesGroup = nullptr;

public:
	virtual void SetParameters(gs_effect_t *effect)
	{
		UNUSED_PARAMETER(effect);
	}
	virtual void SetProperties(obs_properties_t *mainProperties,
				   std::string name)
	{
		UNUSED_PARAMETER(mainProperties);
		UNUSED_PARAMETER(name);
	}
	virtual void UpdateShader(obs_data_t *settings)
	{
		UNUSED_PARAMETER(settings);
	}
	virtual void SetPropertyDefaults(obs_data_t *settings)
	{
		UNUSED_PARAMETER(settings);
	}
	virtual void Render(obs_source_t *context)
	{
		UNUSED_PARAMETER(context);
	}
	virtual void HidePropertiesGroup() {}
	virtual void ShowPropertiesGroup() {}
	virtual const char *GetShaderFilePath()
	{
		const char *path = obs_module_file(shaderFileName);
		return path;
	}
	virtual const char *GetShaderName() { return shaderFileName; }
};

#endif // BASESHADER_H
