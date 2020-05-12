require 'nokogiri'

module Xml2md
  class CompoundBase
    def initialize(elem)
      @elem = elem
    end

    def self.key(sym, type, name: nil, klass: nil, singleton: false)
      raise "invalid type: #{type}" unless [ :attribute, :text, :child ].include?(type)

      @keys ||= {}
      @keys[sym] = {
        type: type,
        name: name.nil? ? sym.to_s : name.to_s,
        klass: klass,
        singleton: singleton
      }
    end

    def self.keys
      @keys
    end

    def [](key)
      raise "no such key: #{key}" unless self.class.keys[key]

      case self.class.keys[key][:type]
      when :attribute then on_attribute(key)
      when :text      then on_text(key)
      when :child     then on_child(key)
      else raise "unhandled key-type: #{self.class.keys[key][:type]}"
      end
    end

    def method_missing(symbol, *args)
      super if self.class.keys[symbol].nil?
      define_singleton_method(symbol) { send(:[], symbol) }
      send(symbol)
    end

    private

    def on_attribute(key)
      k = self.class.keys[key]
      @elem[k[:name]]
    end

    def on_text(key)
      k = self.class.keys[key]
      @elem.xpath(k[:name]).first&.content&.strip
    end

    def on_child(key)
      k = self.class.keys[key]
      klass = k[:klass]
      singleton = k[:singleton]

      if singleton
        klass.new(@elem.xpath(k[:name]).first)
      else
        @elem.xpath(k[:name]).map { |elem| klass.new(elem) }
      end
    end
  end
end
