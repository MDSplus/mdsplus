import defaults from 'lodash/defaults';

import React, { ChangeEvent, PureComponent } from 'react';
import { LegacyForms } from '@grafana/ui';
import { QueryEditorProps } from '@grafana/data';
import { DataSourceStream } from './DataSourceStream';
import { defaultQuery, StreamDataSourceOptions, StreamQuery } from './types';

const { FormField } = LegacyForms;
type Props = QueryEditorProps<DataSourceStream, StreamQuery, StreamDataSourceOptions>;

export class QueryEditor extends PureComponent<Props> {
  onLabelChange = (event: ChangeEvent<HTMLInputElement>) => {
    const { onChange, query } = this.props;
    onChange({ ...query, label: event.target.value });
  };
  onSignalChange = (event: ChangeEvent<HTMLInputElement>) => {
    const { onChange, query } = this.props;
    onChange({ ...query, signal: event.target.value });
  };
  onApply = (event: React.MouseEvent<HTMLElement>) => {
    const { onRunQuery } = this.props;
    // executes the query
    onRunQuery();
  };
  render() {
    const query = defaults(this.props.query, defaultQuery);
    const { signal, label } = query;

    return (
      <div className="gf-form">
        <FormField width={8} value={signal} onChange={this.onSignalChange} label="Signal" type="string" />
        <FormField width={8} value={label} onChange={this.onLabelChange} label="Label" type="string" />
        <button className="square" onClick={this.onApply}>
          Apply
        </button>
      </div>
    );
  }
}
