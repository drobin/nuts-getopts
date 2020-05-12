require_relative './compound_base'

module Xml2md
  class Location < CompoundBase
    key :file, :attribute
    key :line, :attribute
    key :column, :attribute
  end

  class Param < CompoundBase
    key :type, :text
    key :declname, :text
    key :array, :text
  end

  class Function < CompoundBase
    key :id, :attribute
    key :type, :text
    key :definition, :text
    key :argsstring, :text
    key :name, :text
    key :briefdescription, :text
    key :detaileddescription, :text
    key :inbodydescription, :text
    key :location, :child, klass: Location, singleton: true
    key :param, :child, klass: Param
  end

  class EnumValue < CompoundBase
    key :id, :attribute
    key :name, :text
    key :briefdescription, :text
    key :detaileddescription, :text
  end

  class Enum < CompoundBase
    key :id, :attribute
    key :type, :text
    key :name, :text
    key :briefdescription, :text
    key :detaileddescription, :text
    key :inbodydescription, :text
    key :location, :child, klass: Location, singleton: true
    key :enumvalue, :child, klass: EnumValue
  end

  class Member < CompoundBase
    key :id, :attribute
    key :type, :text
    key :definition, :text
    key :name, :text
    key :briefdescription, :text
    key :detaileddescription, :text
    key :inbodydescription, :text
    key :location, :child, klass: Location, singleton: true
  end

  class Struct < CompoundBase
    key :id, :attribute
    key :name, :text, name: :compoundname
    key :briefdescription, :text
    key :detaileddescription, :text
    key :member, :child, klass: Member, name: "sectiondef/memberdef"

    def typedef?
      [ "nuts_getopts_state" ].include?(name)
    end
  end
end
