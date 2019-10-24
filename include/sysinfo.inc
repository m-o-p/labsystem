<?php
throw new Exception("Use header.inc instead of sysinfo.inc");

// Login/Logout
if ($usr->isOfKind(IS_USER)) {
    $lo_text = $lng->get("MnuEntryLogOut") . ': ' . $usr->userName;
    $lo_link = $url->link2($labsystem_LogOutLink);
} else {
    $lo_text = $lng->get("MnuEntryLogIn");
    $lo_link = $url->link2($labsystem_LogInLink);
}


// seeing someone
$seeing_link_all = $url->link2(NULL, Array('seeMe' => 'all'), removeFromArray($_GET, 'hideAnswers'));

if ($url->available('hideAnswers')) {
    $seeing_link_enable = $url->link2(NULL, NULL, removeFromArray($_GET, 'hideAnswers'));
    $seeing_action = "fa-square";
    $seeing_link = false;
} else {
    $seeing_link_enable = $url->link2(NULL, Array('hideAnswers' => 'true'), $_GET);
    $seeing_action = "fa-check-square";

    if (isset($_GET['seeMe'])) {
        $seeing_link = $url->link2(NULL, NULL, removeFromArray($_GET, 'seeMe'));
        $seeing_of = $usr->theSeeingDescr();
    } else {
        $seeing_of = "–";
    }
}

// obfuscate user names
$pseudoname_link = "";
$pseudoname_state = "";

if ($usr->isOfKind(IS_CORRECTOR)) {
    if ($url->available('pseudoNames')) {
        $pseudoname_link = $url->link2(NULL, NULL, removeFromArray($_GET, 'pseudoNames'));
        $pseudoname_state = "fa-check-square";
    } else {
        $pseudoname_link = $url->link2(NULL, Array('pseudoNames' => 'true'), $_GET);
        $pseudoname_state = "fa-square";
    }
}

// temporarily hide example solutions
$solutions_link = "";
$solutions_state = "";

if ($usr->isOfKind(IS_EX_SOLUTION_VIEWER)) {
    if ($url->available('hideExampleSolutionInThisTab')) {
        $solutions_link = $url->link2(NULL, NULL, removeFromArray($_GET, 'hideExampleSolutionInThisTab'));
        $solutions_state = "fa-square";
    } else {
        $solutions_link = $url->link2(NULL, Array('hideExampleSolutionInThisTab' => 'true'), $_GET);
        $solutions_state = "fa-check-square";
    }
}

// wysiwyg
if ($usr->isOfKind(IS_CONTENT_EDITOR) && $cfg->doesExist('tinyMCE') && ($cfg->get('tinyMCE') == 'TRUE')) {
    $wysiwyg_show = true;
    if ($url->available('tinyMCE')) {
        $wysiwyg_url = $url->link2(NULL, NULL, removeFromArray($_GET, 'tinyMCE'));
        $wysiwyg_state = "fa-check-square";
    } else {
        $wysiwyg_url = $url->link2(NULL, Array('tinyMCE' => 'true'), $_GET);
        $wysiwyg_state = "fa-square";
    }
} else {
    $wysiwyg_show = false;
}


// language selector
$lang_link = $url->link2('../pages/selectSysLng.php');

$template = $twig->load('parts/header.html.twig');
$template->displayBlock('header', ['test1'=> "footest"]);

?>

<nav class="navbar navbar-dark topnav navbar-expand py-0">
    <ul class="navbar-nav ml-auto align-items-center">
        <li class="nav-item">
            <a class="nav-link" href="<?php echo $lo_link; ?>"><?php echo $lo_text; ?></a>
        </li>
        <li class="nav-item <?php echo($usr->isOfKind(IS_CORRECTOR) ? '' : 'd-none'); ?>">
            <span>
                <a class="nav-link" href="<?php echo $seeing_link_enable; ?>">
                    <i class="far <?php echo $seeing_action; ?>"></i> <?php echo $lng->get("dataOf"); ?>
                </a>
                <?php echo $seeing_of; ?>

                <a class="nav-link" href="<?php echo $seeing_link; ?>" class="<?php echo(isset($_GET['seeMe']) ? '' : 'd-none'); ?>">
                    <i class="fas fa-times"></i>
                </a>
            </span>
        </li>
        <li class="nav-item <?php echo($usr->isOfKind(IS_CORRECTOR) ? '' : 'd-none'); ?>">
            <a class="nav-link" href="<?php echo $seeing_link_all; ?>"><?php echo $lng->get("forceShowAllUsersData"); ?></a>
        </li>
        <li class="nav-item <?php echo($usr->isOfKind(IS_CORRECTOR) ? '' : 'd-none'); ?>">
            <a class="nav-link" href="<?php echo $pseudoname_link; ?>">
                <i class="far <?php echo $pseudoname_state; ?>"></i> <?php echo $lng->get('pseudoNames'); ?>
            </a>
        </li>
        <li class="nav-item <?php echo($usr->isOfKind(IS_EX_SOLUTION_VIEWER) ? '' : 'd-none'); ?>">
            <a class="nav-link" href="<?php echo $solutions_link; ?>">
                <i class="far <?php echo $solutions_state; ?>"></i> <?php echo $lng->get('showExampleSolutionInThisTab'); ?>
            </a>
        </li>
        <li class="nav-item <?php echo($wysiwyg_show ? '' : 'd-none'); ?>">
            <a class="nav-link" href="<?php echo $wysiwyg_url; ?>">
                <i class="far <?php echo $wysiwyg_state; ?>"></i> WYSIWYG
            </a>
        </li>
        <li class="nav-item">
            <a class="nav-link" href="<?php echo $lang_link; ?>">
                <?php echo $runningSystemLanguage; ?>
            </a>
        </li>
    </ul>
</nav>
