#coding = utf-8
import sys
import os


class PluginTemplateGenerator:
    def __init__(self, name, path):
        self.__plugin_name = name
        self.__output_path = path

    def _replace_str(self, source):
        dst = source.replace('%plugin%', self.__plugin_name)
        dst = dst.replace('%pluginl%', self.__plugin_name.lower())
        dst = dst.replace('%plugindef%', self.__plugin_name.upper())
        return dst

    def set_plugin_name(self):
        pass

    def generate(self):
        template_path = os.path.join(os.path.dirname(sys.argv[0]), 'template')
        files = {'plugin_template.h': '{0}.h'.format(self.__plugin_name.lower()),
                 'plugin_template.cpp': '{0}.cpp'.format(self.__plugin_name.lower()),
                 'pluginspec_template.cpp': '{0}spec.cpp'.format(self.__plugin_name.lower()),
                 'plugin_template.pro': '{0}.pro'.format(self.__plugin_name.lower())}
        for k, v in files.items():
            fin = open(os.path.join(template_path, k), 'r')
            dst = list()
            for s in fin.readlines():
                dst.append(self._replace_str(s))
            fin.close()
            fout = open(os.path.join(self.__output_path, v), 'w')
            fout.writelines(dst)
            fout.close()


