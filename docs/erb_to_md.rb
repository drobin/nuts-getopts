require 'erb'
require 'ostruct'

class ErbToMd
  def initialize(source, target, github_optimization: false)
    @source = source
    @target = target
    @github_optimization = github_optimization
  end

  def run
    template = ERB.new(IO.read(@source), nil, "<>")
    IO.write(@target, template.result(mk_context.get_binding))
  end

  private

  def mk_context
    project_root = File.join(File.dirname(__FILE__), "..")
    src_dir = File.join(project_root, "src")
    examples_dir = File.join(src_dir, "examples")
    context = OpenStruct.new(github: @github_optimization)

    class << context
      def get_binding
        binding
      end
    end

    Dir[File.join(examples_dir, "*.c")].inject(context) do |a, e|
      key = "#{File.basename(e, ".c")}_c"
      a[key] = e
      a
    end
  end
end
