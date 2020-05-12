require 'nokogiri'

module Xml2md
  class Context
    def initialize(path)
      @path = path
    end

    def select(type)
      send("select_#{type}")
    end

    def get_binding
      binding
    end

    private

    def doc
      @doc ||= File.open(@path) { |f| Nokogiri::XML(f) }
    end

    def select_functions
      doc.xpath("/doxygen/compounddef/sectiondef[@kind='func']/memberdef").map do |memberdef|
        Function.new(memberdef)
      end
    end

    def select_enums
      doc.xpath("/doxygen/compounddef/sectiondef[@kind='enum']/memberdef").map do |memberdef|
        Enum.new(memberdef)
      end
    end

    def select_structs
      doc.xpath("/doxygen/compounddef/innerclass").map do |innerclass|
        innerpath = File.join(File.dirname(@path), "#{innerclass["refid"]}.xml")
        innerdoc = File.open(innerpath) { |f| Nokogiri::XML(f) }
        compounddef = innerdoc.xpath("/doxygen/compounddef")
        Struct.new(compounddef.first)
      end
    end
  end
end
