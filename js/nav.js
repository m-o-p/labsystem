function init_sidenav_toggle() {
    var sidenav =  $('#sidenav');
    var lstorageid = "labsystem:sidenav_hidden";
    var open = true;
    try {
        open = localStorage.getItem(lstorageid) === "true";
    } catch (e) {
    }

    sidenav.addClass("no-transition");
    sidenav.toggleClass('collapsed', open);

    $('#sidenav-toggler').click(function (e) {
        var open = !sidenav.hasClass('collapsed');
        sidenav.toggleClass('collapsed', open);
        localStorage.setItem(lstorageid, open.toString());
    });
    sidenav.removeClass("no-transition");
}

function init_persist_sidenavstate() {
    // Persist nav open states
    var sidenav =  $('#sidenav');
    var lstorageid = "labsystem:sidenav";
    var open = [];
    var breakpoint_sm = parseInt(getComputedStyle(document.body).getPropertyValue('--breakpoint-sm').replace("px", ""));

    try {
        open = JSON.parse(localStorage.getItem(lstorageid) || "[]");
    } catch (e) {
    }

    $('#sidenav .collapse').on('shown.bs.collapse hidden.bs.collapse', function (e) {
        var id = $(this).attr('id');
        if (e.type === "hidden") {
            open.splice(open.indexOf(id), 1);
        } else if (e.type === "shown") {
            if (open.indexOf(id) === -1) {
                open.push(id);
            }
        }
        localStorage.setItem(lstorageid, JSON.stringify(open));
    });

    // Hide Sidenav after clicking on a link on mobile (sidenav.width == 100%)
    $("#sidenav a.nav-link:not(.subnavctrl)").on('click', function () {
        if (breakpoint_sm >= window.innerWidth) {
            $('#sidenav-toggler').trigger('click');
        }
    });

    sidenav.addClass('no-transition')
    $.each(open, function (_, elt) {
        $("#" + elt).collapse('show');
    });
    sidenav.removeClass("no-transition");
}

init_sidenav_toggle();
init_persist_sidenavstate();
