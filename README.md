# Ashmanix Fun Source OBS Plugin

## Introduction

This plugin is designed to allow you to add a source that uses various shaders.

This plugin is designed to be used with `OBS version 28` and above.

## Usage

To use this plugin:
TBC

## Build Instructions

To build this code use the included scripts that are taken from the [OBS Plugin Template](https://github.com/obsproject/obs-plugintemplate).

### GitHub Actions & CI

The scripts contained in github/scripts can be used to build and package the plugin and take care of setting up obs-studio as well as its own dependencies. A default workflow for GitHub Actions is also provided and will use these scripts.

#### Retrieving build artifacts

Each build produces installers and packages that you can use for testing and releases. These artifacts can be found on the action result page via the "Actions" tab in your GitHub repository.

#### Building a Release

Simply create and push a tag and GitHub Actions will run the pipeline in Release Mode. This mode uses the tag as its version number instead of the git ref in normal mode.