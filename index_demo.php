<?php
/**
 *  labsystem.m-o-p.de - 
 *                  the web based eLearning tool for practical exercises
 *  Copyright (C) 2010  Marc-Oliver Pahl
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *
 * FOR NORMAL USE YOU DO NOT NEED THIS FILE!
 *
 *
 * This script is used for the demo on http://labsystem.m-o-p.de/
 * It calls php/emptyDB.php?config=demoX where X is alternating
 * in [0..4].
 * To save the current state it uses the file counter.txt that
 * must exist in the labsystem's root directory and be read and 
 * writable by php.
 *
 * /php/emptyDB.php contains further information on this.
 *
 */
 $fileName = 'counter.txt';
 
 // get current value
 $fileHandle = fopen( $fileName, "rb" );
 $currentIdx = fread ( $fileHandle, filesize ( $fileName ) );
 fclose ( $fileHandle );

 // set it to the next value
 $fileHandle = fopen ( $fileName, 'w+' );
 fwrite( $fileHandle, ($currentIdx % 5) + 1 );
 fclose( $fileHandle );

 // PUT the CONFIGURATION you want to use at the END!
 // e.g. config_course23.ini -> &config=course23
 header ('Location: http://labsystem.m-o-p.de/php/emptyDB.php?config=demo'.$currentIdx.'&userrole='.$_GET['userrole']);

?>