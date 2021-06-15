import { DataQuery, DataSourceJsonData } from '@grafana/data';

export interface MyQuery extends DataQuery {
  experiment?: string;
  shot?: number;
  signal?: string;
}

export const defaultQuery: Partial<MyQuery> = {
  experiment: '',
  signal: '',
  shot: -1,
};

/**
 * These are options configured for each DataSource instance
 */
export interface MyDataSourceOptions extends DataSourceJsonData {
  path?: string;
  port?: number;
  ipAddress?: string;
}

/**
 * Value that is used in the backend, but never sent over HTTP to the frontend
 */
export interface MySecureJsonData {
  apiKey?: string;
}
