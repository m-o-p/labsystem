{% macro view(address) %}
  <a tabindex="{{nextTabIndex()}}" href="{{link2("../pages/view.php?address=" ~ address ~ '#' ~ address)}}">
    <img src="../syspix/button_view_13x12.gif" width="13" height="12" border="0" alt="view" title="{{"explainView"|t}}">
  </a>
{% endmacro %}

{% macro edit(address) %}
  <a tabindex="{{nextTabIndex()}}" href="{{link2("../pages/edit.php?address=" ~ address)}}">
    <img src="../syspix/button_edit_13x12.gif" width="13" height="12" border="0" alt="edit" title="{{"explainEdit"|t}}">
  </a>
{% endmacro %}

{% macro print(address) %}
  <a tabindex="{{nextTabIndex()}}" href="{{link2("../pages/view.php?address=" ~ address)}}">
    <img src="../syspix/button_folder_open_13x12.gif" width="13" height="12" border="0" alt="print" title="{{"explainPrintView"|t}}">
  </a>
{% endmacro %}

{% macro create(elementID) %}
  <a tabindex="{{nextTabIndex()}}" href="{{link2("../pages/edit.php?address=" ~ address~ "1&saveAsNew=true")}}">
    <img src="../syspix/button_new_13x12.gif" width="13" height="12" border="0" alt="new" title="{{"explainCreateNew"|t}}">
  </a>
{% endmacro %}

{% macro manageElements() %}
  <a tabindex="{{nextTabIndex()}}" href="{{link2("../pages/view.php?address=p2")}}">
    <img src="../syspix/button_manage_elements_13x12.gif" width="13" height="12" border="0" alt="manage" title="{{"explainManageElmnts"|t}}">
  </a>
{% endmacro %}

{% macro delete(address) %}
  <a tabindex="{{nextTabIndex()}}" href="{{link2("../pages/delete.php?address=" ~ address)}}" class="confirmDelete">
    <img src="../syspix/button_delete_13x12.gif" width="13" height="12" border="0" alt="delete" title="{{"explainDeleteElemnt"|t}}">
  </a>
{% endmacro %}

{% macro viewStructure(address) %}
  <a tabindex="{{nextTabIndex()}}" href="{{link2("../pages/view.php?address=" ~ address ~ ".structure")}}">
    <img src="../syspix/button_view_structure_13x12.gif" width="13" height="12" border="0" alt="view structure" title="{{"explainViewStructure"|t}}">
  </a>
{% endmacro %}

{% macro link2Element(address, element) %}
  <div class="hiddenDiv">
    <a href="../pages/view.php?address={{address}}&amp;amp;&#x005F;_LINKQUERY__" class="hiddenDivLink">
      <img src="../syspix/button_link2_13x12.gif" width="13" height="12" border="0" alt="link to" title="{{"explainLink2"|t}}">
    </a>
    <div class="hiddenDivContents">
      <input type="text" style="width: 100%" value="<a href=""></a>" class="clickSelect">

      {% if element.title %}
      {% endif %}
    </div>
  </div>
{% endmacro %}

{% macro link2Epub(address) %}
  {% if "allowEpubDownload"|cfg %}
    <a tabindex="{{nextTabIndex()}}" href="{{link2("../pages/view.php?address=" ~ address ~ ".epub&__LINKQUERY__")}}">
      <img src="../syspix/button_epub_12x12.gif" width="12" height="12" border="0" alt="link to epub" title="{{"explainLink2epub"|t}}">
    </a>
  {%endif%}
{% endmacro %}

{% macro history(history, address, sendAsAddHist) %}
  TODO
{% endmacro %}
