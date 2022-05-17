classdef MDSplusAPI < handle
% MDSplusAPI new API core for MDSplus
% e.g.:
% - read signal from remote tree on mds-data-1 using thick client
%  setenv('default_tree_path', 'mds-data-1::')
%  mdsip = MDSplusAPI();
%  mdsip.open('mytree', 7357);
%  sig = mdsip.tree.getNode('.signals:channel43').getRecord();
%  dat = sig.toFloatArray();
%  dim = sig.getDimension().toFloatArray();
%  plot(dim, dat)
% - using thin client connection to mds-data-1
%  mdsip = MDSplusAPI('mds-data-1')
%  ...

    properties(SetAccess = private)
        con;    % java object of the connection
        api;    % java object of the api
        tree;   % java object of the current tree
    end
    methods (Static)
        function null = null()
            % NULL convenient access to java null
            %   arr = mdsip.toArray(mdsip.compile('1UQ'));
            %   res = mdsip.api.tdiExecute(mdsip.null, 'FLOAT($)', arr);
            %   data = res.getData();
            null = [];
        end
        function arr = toArray(varargin)
            % NULL convenient way to create Descriptor[]
            %   arr = mdsip.toArray(mdsip.compile('1UQ'));
            arr = javaArray('mds.data.descriptor.Descriptor', nargin);
            for i = 1 : nargin
                arr(i) = varargin{i};
            end
        end
    end
    methods
        function self = MDSplusAPI(varargin)
            % MDSPLUSAPI connects to local mdsip spawn, tcp-, or ssh-server
            % 	% spawn local mdsip session
            %   local = MDSplusAPI();
            %   % tcp connection to server
            %   mdsip = MDSplusAPI('mds-data-1');
            %   % ssh connection to server mds-data-1 as user johndoe
            %   mdsssh = MDSplusAPI('ssh://johndoe@mds-data-1');
            persistent java;
            if isempty(java)
                javaaddpath(fullfile(getenv('MDSPLUS_DIR'), ...
                    'java', 'classes', 'mdsplus-api.jar'))
                java = 1;
            end
            import('mds.mdsip.MdsIp');
            self.con = mds.mdsip.MdsIp(varargin{:});
            self.con.connect();
            self.api = self.con.getAPI();
            self.tree = [];
        end
        function delete(self)
            % DELETE  closes the connection object
            %   delete is called automatically when instance is GCed
            self.con.close();
        end
        function open(self, expt, shot, varargin)
            % OPEN  opens a tree optional with write access
            %   mdsip.open('mytree', 7357, true)
            %   node = mdsip.tree.getNode('DATA')
            import('mds.data.TREE');
            newtree = mds.data.TREE(self.con, expt, shot);
            newtree.open(varargin{:});
            self.close();
            self.tree = newtree;
        end
        function close(self)
            % CLOSE  closes current .tree if open
            if ~isempty(self.tree)
                try
                    self.tree.close();
                catch
                end
                self.tree = [];
            end
        end
        function obj = compile(self, expr, varargin)
            % COMPILE  creates mdsplus objects from tdi expression
            %   obj = mdsip.compile('BUILD_WITH_UNITS(1UQ, "ns")');
            arr = MDSplusAPI.toArray(varargin{:});
            data = self.api.tdiCompile(self.tree, expr, arr);
            obj = data.getData();
        end
        function obj = execute(self, expr, varargin)
            % EXECUTE  execute tdi expression with optional $ args
            %   ag1 = mdsip.compile('BUILD_WITH_UNITS(1UQ, "ns")');
            %   obj = mdsip.execute('FLOAT($)', ag1);
            arr = MDSplusAPI.toArray(varargin{:});
            data = self.api.tdiExecute(self.tree, expr, arr);
            obj = data.getData();
        end
        function obj = evaluate(self, obj)
            % EVALUATE  evaluate mdsplus object
            %   obj = mdsip.evaluate(obj_in);
            data = self.api.tdiEvaluate(self.tree, obj);
            obj = data.getData();
        end
    end
end
