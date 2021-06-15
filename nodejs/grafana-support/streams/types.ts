import { DataQuery, DataSourceJsonData } from '@grafana/data';

export interface StreamQuery extends DataQuery {
  port?: number;
  signal?: string;
  label?: string;
}

export const defaultQuery: Partial<StreamQuery> = {
  signal: '',
};

/**
 * These are options configured for each DataSource instance
 */
export interface StreamDataSourceOptions extends DataSourceJsonData {
  port?: number;
  path?: string;
}

/**
 * Value that is used in the backend, but never sent over HTTP to the frontend
 */
export interface StreamSecureJsonData {
  apiKey?: string;
}
