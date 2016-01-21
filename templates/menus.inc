{% import "buttons.inc" as buttons %}

{% macro top(element, address, shortcut) %}
  {% if IS_CONTENT_EDITOR is allowed %}
    <div class="labsys_mop_elements_menu_{{shortcut}}">
      {% if element.visibleOnlyInCollection %}
        <span class="labsys_mop_element_note_in_header">{{"visibleOnlyColl"|t}}</span>
      {% endif %}
      <a href="#{{address}}">
        <span class="labsys_mop_element_description_in_header">
          {{adress}}
        </span>
      </a>
      {{buttons.link2Element(address, element)}}
      {{buttons.edit(address)}}
      {{buttons.create(address)}}
      {{buttons.delete(address)}}
      {{buttons.manageElements(address)}}
      {{buttons.mail(address, element.elementId ~ element.idx ~ ": " ~ element.title)}}
      {{buttons.history(element.history, address)}}
    </div>
  {% endif %}
{% endmacro %}

{% macro bottom(element, address, shortcut) %}
  {% if IS_CONTENT_EDITOR is allowed %}
    <div class="labsys_mop_elements_menu_{{shortcut}}_edit_bottom"></div>
  {% else %}
    <div class="labsys_mop_elements_menu_{{shortcut}}_edit_bottom">
      {{buttons.mail(address, element.elementId ~ element.idx ~ ": " ~ element.title)}}
      {{buttons.history(element.history, address)}}
    </div>
  {% endif %}
{% endmacro %}

{% macro topSchedule(element, address) %}
  {% if IS_CONTENT_EDITOR is allowed or IS_SCHEDULER is allowed %}
    <div class="labsys_mop_elements_menu_s">
      {% if IS_CONTENT_EDITOR is allowed %}
        {#{{buttons.link2Element("s1", element)}}#}
      {% else %}
        {{butons.create("s")}}
      {% endif %}
    </div>
  {% endif %}
{% endmacro %}

{% macro editTop(element, address, shortcut) %}
  TODO
{% endmacro %}

{% macro editBottom(element, address, shortcut) %}
  {% import _self as menus %}

  {{ menus.editTop(element, address, shortcut) }}
{% endmacro %}
