import defaults from 'lodash/defaults';

import React, { ChangeEvent, PureComponent } from 'react';
import { LegacyForms } from '@grafana/ui';
import { QueryEditorProps } from '@grafana/data';
import { DataSource } from './DataSource';
import { defaultQuery, MyDataSourceOptions, MyQuery } from './types';

const { FormField } = LegacyForms;
type Props = QueryEditorProps<DataSource, MyQuery, MyDataSourceOptions>;

export class QueryEditor extends PureComponent<Props> {
  onExperimentChange = (event: ChangeEvent<HTMLInputElement>) => {
    const { onChange, query } = this.props;
    onChange({ ...query, experiment: event.target.value });
  };
  onSignalChange = (event: ChangeEvent<HTMLInputElement>) => {
    const { onChange, query } = this.props;
    onChange({ ...query, signal: event.target.value });
  };
  onShotChange = (event: ChangeEvent<HTMLInputElement>) => {
    const { onChange, query } = this.props;
    onChange({ ...query, shot: parseInt(event.target.value, 10) });
    // executes the query
    //  onRunQuery();
  };
  onApply = (event: React.MouseEvent<HTMLElement>) => {
    const { onRunQuery } = this.props;
    // executes the query
    onRunQuery();
  };
  render() {
    const query = defaults(this.props.query, defaultQuery);
    const { experiment, shot, signal } = query;

    return (
      <div className="gf-form">
        <FormField width={8} value={experiment} onChange={this.onExperimentChange} label="Experiment" type="string" />
        <FormField width={4} value={shot} onChange={this.onShotChange} label="Shot" type="number" />
        <FormField width={8} value={signal} onChange={this.onSignalChange} label="Signal" type="string" />
        <button className="square" onClick={this.onApply}>
          Apply
        </button>
      </div>
    );
  }
}
