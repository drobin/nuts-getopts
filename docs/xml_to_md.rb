require 'erb'

Dir[File.dirname(__FILE__) + "/xml_to_md/*.rb"].each { |f| require f }

class XmlToMd
  def initialize(xml_source, erb_source, target)
    @xml_source = xml_source
    @erb_source = erb_source
    @erb_target = target
  end

  def run
    template = ERB.new(IO.read(@erb_source), nil, "<>")
    context = Xml2md::Context.new(@xml_source)
    IO.write(@erb_target, template.result(context.get_binding))
  end
end
