import { DataSourcePlugin } from '@grafana/data';
import { DataSourceStream } from './DataSourceStream';
import { ConfigEditor } from './ConfigEditor';
import { QueryEditor } from './QueryEditor';
import { StreamQuery, StreamDataSourceOptions } from './types';

export const plugin = new DataSourcePlugin<DataSourceStream, StreamQuery, StreamDataSourceOptions>(DataSourceStream)
  .setConfigEditor(ConfigEditor)
  .setQueryEditor(QueryEditor);
