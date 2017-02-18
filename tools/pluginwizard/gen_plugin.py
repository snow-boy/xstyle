from plugin_template_generator import PluginTemplateGenerator
import sys
import os


if __name__ == '__main__':  
    plugin_name = sys.argv[1];
    plugin_dir = str()
    if len(sys.argv) > 2:
	    plugin_dir = sys.argv[2]
    else:
	    plugin_dir = os.path.join(os.path.dirname(sys.argv[0]), '..', '..' , 'modules')
    plugin_path = os.path.join(plugin_dir, plugin_name.lower())
    os.mkdir(plugin_path)
    PluginTemplateGenerator(plugin_name, plugin_path).generate()