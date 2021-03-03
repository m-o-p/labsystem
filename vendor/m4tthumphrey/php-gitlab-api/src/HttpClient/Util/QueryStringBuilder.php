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

namespace Gitlab\HttpClient\Util;

/**
 * @internal
 */
final class QueryStringBuilder
{
    /**
     * Encode a query as a query string according to RFC 3986.
     *
     * Indexed arrays are encoded using empty squared brackets ([]) unlike
     * `http_build_query`.
     *
     * @param array $query
     *
     * @return string
     */
    public static function build(array $query): string
    {
        return \sprintf('?%s', \implode('&', \array_map(function ($value, $key): string {
            return self::encode($value, $key);
        }, $query, \array_keys($query))));
    }

    /**
     * Encode a value.
     *
     * @param mixed  $query
     * @param scalar $prefix
     *
     * @return string
     */
    private static function encode($query, $prefix): string
    {
        if (!\is_array($query)) {
            return self::rawurlencode($prefix).'='.self::rawurlencode($query);
        }

        $isList = self::isList($query);

        return \implode('&', \array_map(function ($value, $key) use ($prefix, $isList): string {
            $prefix = $isList ? $prefix.'[]' : $prefix.'['.$key.']';

            return self::encode($value, $prefix);
        }, $query, \array_keys($query)));
    }

    /**
     * Tell if the given array is a list.
     *
     * @param array $query
     *
     * @return bool
     */
    private static function isList(array $query): bool
    {
        if (0 === \count($query) || !isset($query[0])) {
            return false;
        }

        return \array_keys($query) === \range(0, \count($query) - 1);
    }

    /**
     * Encode a value like rawurlencode, but return "0" when false is given.
     *
     * @param mixed $value
     *
     * @return string
     */
    private static function rawurlencode($value): string
    {
        if (false === $value) {
            return '0';
        }

        return \rawurlencode((string) $value);
    }
}
