{% macro hiddenInput(name, value) %}
  <input type="hidden" name="{{name}}" value="{{value}}">
{% endmacro %}

{% macro label(for, text, class) %}
  {% if class %}
    <label for="{{for}}" class="labsys_mop_input_field_label_{{class}}">
      {{text|t}}
    </label>
  {% else %}
    <label for="{{for}}" class="labsys_mop_input_field_label">
      {{text|t}}
    </label>
  {% endif %}
{% endmacro %}

{% macro select(name, options) %}
  <select tabindex="{{nextTabIndex()}}" id="{{name}}" name={{name}} size="1" class="labsys_mop_input">
    {{options|raw}}
  </select>
{% endmacro %}

{% macro option(value, selected) %}
  <option {{selected?"selected":""}}>{{value}}</option>
{% endmacro %}

{% macro textarea(name, value) %}
  <textarea tabindex="{{nextTabIndex()}}" name="{{name}}" rows={{"DefaultSCommentRows"|t}} class="labsys_mop_textarea">
    {{- value}}
  </textarea>
{% endmacro %}

{% macro submit(str) %}
  <input tabindex="{{nextTabIndex()}}" type="submit" class="labsys_mop_button" value="{{str|t}}">
{% endmacro %}

{% macro checkbox(str) %}
  <input tabindex="{{nextTabIndex()}}" type="checkbox" name="{{name}}">
{% endmacro %}
