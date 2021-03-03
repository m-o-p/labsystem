<?php

declare(strict_types=1);

/*
 * This file is part of the Gitlab API library.
 *
 * (c) Matt Humphrey <matth@windsor-telecom.co.uk>
 * (c) Graham Campbell <graham@alt-three.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace Gitlab\Api;

class ProjectNamespaces extends AbstractApi
{
    /**
     * @param array $parameters {
     *
     *     @var string $search Returns a list of namespaces the user is authorized to see based on the search criteria.
     * }
     *
     * @return mixed
     */
    public function all(array $parameters = [])
    {
        $resolver = $this->createOptionsResolver();
        $resolver->setDefined('search');

        return $this->get('namespaces', $resolver->resolve($parameters));
    }

    /**
     * @param int|string $namespace_id
     *
     * @return mixed
     */
    public function show($namespace_id)
    {
        return $this->get('namespaces/'.self::encodePath($namespace_id));
    }
}
