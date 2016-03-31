from wtforms.widgets import HTMLString


def InputGroupWidget(field, **kwargs):
    html = ['<div class="input-group minidisplayedit"> ']

    for subfield in field:
        html.append(subfield(class_="form-control"))

    html.append('</div>')
    return HTMLString(''.join(html))
