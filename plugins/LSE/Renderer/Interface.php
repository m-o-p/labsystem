<?php
interface LSE_Renderer_Interface
{
    public function __construct(LSE_Engine $engine);
    public function render();
}